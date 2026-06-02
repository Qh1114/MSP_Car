#include "Maze.h"
#include <string.h>
#include "Delay.h"
#include "tb6612.h" //到时候可以改成电机编码器控制
#include "Drive.h"
#include "IMU.h"

/*
 * 每个格子用 1 个结构体保存三类信息：
 * 1) wallMask   -> 4 个方向有没有墙
 * 2) visited    -> DFS 是否已经到过
 * 3) distance   -> Flood Fill 中该格子到目标点的距离
 */
// 四个标志位分别表示上右下左是否有墙
#define MAZE_WALL_UP_BIT (1u << 0)
#define MAZE_WALL_RIGHT_BIT (1u << 1)
#define MAZE_WALL_DOWN_BIT (1u << 2)
#define MAZE_WALL_LEFT_BIT (1u << 3)

/* 距离数组的“无穷大”占位值，表示当前还没算到这个格子 */
#define MAZE_INF_DISTANCE (0xFFu)

typedef struct
{
    uint8_t wallMask;
    uint8_t visited;
    uint8_t distance;
} MazeCell; // 单位格子

/* ======================== 全局运行状态 ========================
 * 这些变量相当于迷宫算法的“状态机记忆”：
 * 当前在地图上的位置、朝向、目标点、是否结束、已访问数量等。
 */
static MazeCell s_cells[MAZE_CELL_COUNT];         // 4x4 地图的格子数组
static int8_t s_currentX = 0;                     // 小车当前所在格子的 X 坐标（0~3）
static int8_t s_currentY = 2;                     // 小车当前所在格子的 Y 坐标（0~3）
static int8_t s_goalX = 3;                        // 目标格子的 X 坐标（Flood Fill 的终点）
static int8_t s_goalY = 1;                        // 目标格子的 Y 坐标（Flood Fill 的终点）
static MazeDir s_heading = MAZE_DIR_RIGHT;        // 小车当前朝向（上/右/下/左）
static MazeAlgorithm s_algorithm = MAZE_ALGO_DFS; // 当前选择的算法（DFS 或 Flood Fill）
static uint8_t s_visitedCount = 0u;               // DFS 下已访问格子数量，用于判断是否遍历完 16 格
static bool s_finished = false;                   // 本次任务是否结束（结束后不再走）
static float s_wallThresholdMm = 220.0f;          // 判定“有墙/有挡板”的距离阈值（毫米）
static float s_driveSpeed = 38.0f;                // 电机前进/转向使用的基础速度，注意这里可以改成编码器控制
static bool s_waitingForNextCell = false;         // 已经开始前进，正在等待灰度触发“进入下一格”
static MazeDir s_pendingDir = MAZE_DIR_RIGHT;     // 本次前进对应的目标方向（用于进格后更新坐标）
static bool s_insideMaze = false;                 // 小车是否已经进入迷宫内部（坐标开始计数）
static int8_t s_exitNumber = -1;                  // 出口编号：1..4（最右列自上而下），-1 表示未出或非法

static int8_t s_pathStack[MAZE_CELL_COUNT]; // DFS 回溯栈：记录走过的路径节点
static uint8_t s_pathTop = 0u;              // 回溯栈下标
extern float ypr[3];                        // 来自 IMU.c 的全局变量，实时更新当前姿态（角度单位：度）

/* 把二维坐标压成一维下标，方便用一个长度为 16 的数组保存整张图 */
static int maze_index(int8_t x, int8_t y)
{
    return ((int)y * MAZE_SIZE) + (int)x;
}

/* 判断坐标是否仍在 4x4 地图内部 */
static bool maze_in_range(int8_t x, int8_t y)
{
    return (x >= 0) && (x < MAZE_SIZE) && (y >= 0) && (y < MAZE_SIZE);
}

/* 返回相反方向，用于发现墙时，同时把邻格的反方向也标记成墙 */
static MazeDir maze_opposite_dir(MazeDir dir)
{
    return (MazeDir)(((int)dir + 2) & 0x03); // 相当于%4
}

/* 相对方向转绝对方向，输入当前朝向和相对方向 */
static MazeDir maze_relative_to_absolute(MazeDir heading, MazeDir relative)
{
    return (MazeDir)(((int)heading + (int)relative) & 0x03);
}

/* 绝对方向转相对方向：用于判断“目标方向相对当前朝向”是前进、左转还是右转 */
static MazeDir maze_absolute_to_relative(MazeDir heading, MazeDir absolute)
{
    return (MazeDir)(((int)absolute - (int)heading + 4) & 0x03);
}

/* 把方向换成坐标增量：上(0,-1)、右(1,0)、下(0,1)、左(-1,0) */
static void maze_step_delta(MazeDir dir, int8_t *dx, int8_t *dy)
{
    switch (dir)
    {
    case MAZE_DIR_UP:
        *dx = 0;
        *dy = -1;
        break;
    case MAZE_DIR_RIGHT:
        *dx = 1;
        *dy = 0;
        break;
    case MAZE_DIR_DOWN:
        *dx = 0;
        *dy = 1;
        break;
    default:
        *dx = -1;
        *dy = 0;
        break;
    }
}

/* 把方向映射到 wallMask 的某一位，得到位标准用于标记/查询墙信息 */
static uint8_t maze_wall_bit(MazeDir dir)
{
    switch (dir)
    {
    case MAZE_DIR_UP:
        return MAZE_WALL_UP_BIT;
    case MAZE_DIR_RIGHT:
        return MAZE_WALL_RIGHT_BIT;
    case MAZE_DIR_DOWN:
        return MAZE_WALL_DOWN_BIT;
    default:
        return MAZE_WALL_LEFT_BIT;
    }
}

/* 三路距离值中，只要小于阈值，就认为这个方向前面有墙/挡板 */
static bool maze_sensor_blocked(float distanceMm)
{
    return (distanceMm > 0.0f) && (distanceMm <= s_wallThresholdMm);
}

/* 在当前格子和相邻格子里同时标记一面墙，保证地图信息是对称的 */
static void maze_link_wall(MazeDir dir, int8_t x, int8_t y)
{
    if (!maze_in_range(x, y)) // 判断坐标范围
    {
        return;
    }
    // 有墙的话，在当前格子对应方向上标记1，在邻格的反方向上也标记1
    s_cells[maze_index(x, y)].wallMask |= maze_wall_bit(dir);

    int8_t dx = 0;
    int8_t dy = 0;
    maze_step_delta(dir, &dx, &dy); // 获得下一步的坐标增量

    int8_t nx = (int8_t)(x + dx);
    int8_t ny = (int8_t)(y + dy);

    if (maze_in_range(nx, ny))
    {
        s_cells[maze_index(nx, ny)].wallMask |= maze_wall_bit(maze_opposite_dir(dir));
    }
}

/* 查询某个格子某个方向上是否已经有墙 */
static bool maze_has_wall(int8_t x, int8_t y, MazeDir dir)
{
    if (!maze_in_range(x, y))
    {
        return true;
    }

    return (s_cells[maze_index(x, y)].wallMask & maze_wall_bit(dir)) != 0u;
}

/* DFS 模式下：把当前格子记成“已经访问过” */
static void maze_mark_current_visited(void)
{
    int currentIndex = maze_index(s_currentX, s_currentY);

    if (!s_cells[currentIndex].visited)
    {
        s_cells[currentIndex].visited = 1u;
        if (s_visitedCount < MAZE_CELL_COUNT)
        {
            s_visitedCount++;
        }
    }
}

/* 读取前/左/右三个距离，转成地图里的墙信息 */
static void maze_update_walls_from_sensor(float dis_front, float dis_left, float dis_right)
{
    MazeDir frontDir = s_heading; // 小车绝对朝向
    MazeDir leftDir = maze_relative_to_absolute(s_heading, MAZE_DIR_LEFT);
    MazeDir rightDir = maze_relative_to_absolute(s_heading, MAZE_DIR_RIGHT);

    if (maze_sensor_blocked(dis_front))
    {
        maze_link_wall(frontDir, s_currentX, s_currentY);
    }
    if (maze_sensor_blocked(dis_left))
    {
        maze_link_wall(leftDir, s_currentX, s_currentY);
    }
    if (maze_sensor_blocked(dis_right))
    {
        maze_link_wall(rightDir, s_currentX, s_currentY);
    }
}

/* 转弯函数，提供目标绝对朝向，函数内部会根据当前朝向计算出相对转向，并执行转弯动作。
 * 这里用的是 tb6612 的开环控制，后面如果你接了编码器闭环，可以再替换成更稳的版本。
 */
static void maze_execute_turn(MazeDir targetHeading)
{
    MazeDir relative = maze_absolute_to_relative(s_heading, targetHeading);
    float firstYaw = ypr[1];   // 转弯前的初始航向角（角度单位：度）
    float currentYaw = ypr[1]; // 当前航向角（角度单位：度），根据 IMU 的 Yaw 更新
    float angleerror = 0.5f;   // 转向误差允许范围（度），可以根据实际情况调整
    /* 顺时针 90° / 180° / 逆时针 90° 的简化实现。
     * 当前版本是为了先把整体流程跑起来，后续可以再细化成精确角度控制。
     */
    switch (relative)
    {
    case MAZE_DIR_UP:
        break;
    case MAZE_DIR_RIGHT:
        TB6612_SetLeftMotor(s_driveSpeed);
        TB6612_SetRightMotor(-s_driveSpeed);
        while (1)
        {
            currentYaw = ypr[1];
            if (fabs(angle_diff(currentYaw, firstYaw + 90.0f)) <= angleerror)
            {
                    break;
                }
            Delay_ms(5); // 加个小延时，避免死循环里一直读 IMU 导致性能问题
        }
        break;
    case MAZE_DIR_DOWN:
        TB6612_SetLeftMotor(s_driveSpeed);
        TB6612_SetRightMotor(-s_driveSpeed);
        while (1)
        {
            currentYaw = ypr[1];
            if (fabs(angle_diff(currentYaw, firstYaw + 180.0f)) < angleerror)
            {
                break;
            }
            Delay_ms(5);
        }
        break;
    default:
        TB6612_SetLeftMotor(-s_driveSpeed);
        TB6612_SetRightMotor(s_driveSpeed);
        while (1)
        {
            currentYaw = ypr[1];
            if (fabs(angle_diff(currentYaw, firstYaw - 90.0f)) <= angleerror)
            {
                break;
            }
            Delay_ms(5);
        }
        break;
    }

    TB6612_Brake();
    Delay_ms(50); // 转弯后刹车一下，增加稳定性
    s_heading = targetHeading;
}

/* 转弯后直行等待灰度传感器触发进格 */
static void maze_start_forward_wait_gray(MazeDir targetHeading)
{
    maze_execute_turn(targetHeading);
    TB6612_Forward(s_driveSpeed);
    s_pendingDir = targetHeading;
    s_waitingForNextCell = true;
}

/* 灰度触发进格后，提交一次坐标更新 */
static void maze_commit_forward_step(void)
{
    int8_t dx = 0;
    int8_t dy = 0;
    maze_step_delta(s_pendingDir, &dx, &dy);

    int8_t nx = (int8_t)(s_currentX + dx);
    int8_t ny = (int8_t)(s_currentY + dy);

    /* 如果下一格坐标越界，只有向右出界且 y 在 0..MAZE_SIZE-1 时视为合法出口（最右一列） */
    if (!maze_in_range(nx, ny))
    {
        if ((nx == MAZE_SIZE) && (ny >= 0) && (ny < MAZE_SIZE))
        {
            /* 合法出口：记录出口编号（1..4 从上到下），设置朝向并结束 */
            s_exitNumber = (int8_t)(ny + 1);
            s_heading = s_pendingDir; /* 保证车头指向出口方向 */
            s_finished = true;
            s_insideMaze = false;
            return;
        }

        /* 非法出界：记录为未指定出口并结束任务以防继续越界 */
        s_exitNumber = -1;
        s_heading = s_pendingDir;
        s_finished = true;
        s_insideMaze = false;
        return;
    }

    s_currentX = nx;
    s_currentY = ny;
}

/* 把“下一步要去的绝对方向”翻译成一个动作提示，方便上层打印/调试 */
static MazeAction maze_action_from_turn(MazeDir targetHeading)
{
    MazeDir relative = maze_absolute_to_relative(s_heading, targetHeading);

    switch (relative)
    {
    case MAZE_DIR_UP:
        return MAZE_ACTION_FORWARD;
    case MAZE_DIR_RIGHT:
        return MAZE_ACTION_TURN_RIGHT;
    case MAZE_DIR_DOWN:
        return MAZE_ACTION_TURN_BACK;
    default:
        return MAZE_ACTION_TURN_LEFT;
    }
}

/* Flood Fill 的核心：从目标点开始向外扩散距离值 */
static void maze_rebuild_distance_field(void)
{
    /* 队列用于 BFS 扩散 */
    uint8_t queue[MAZE_CELL_COUNT];
    uint8_t head = 0u;
    uint8_t tail = 0u;

    for (int index = 0; index < MAZE_CELL_COUNT; ++index)
    {
        s_cells[index].distance = MAZE_INF_DISTANCE;
    }

    if (!maze_in_range(s_goalX, s_goalY))
    {
        return;
    }

    /* 目标点距离设为 0，随后把它周围能走到的格子依次标 1、2、3... */
    int goalIndex = maze_index(s_goalX, s_goalY);
    s_cells[goalIndex].distance = 0u;
    queue[tail++] = (uint8_t)goalIndex;

    while (head != tail)
    {
        uint8_t currentIndex = queue[head++];
        int8_t x = (int8_t)(currentIndex % MAZE_SIZE);
        int8_t y = (int8_t)(currentIndex / MAZE_SIZE);
        uint8_t nextDistance = (uint8_t)(s_cells[currentIndex].distance + 1u);

        /* 从当前格子向四个方向扩散 */
        for (MazeDir dir = MAZE_DIR_UP; dir <= MAZE_DIR_LEFT; dir = (MazeDir)((int)dir + 1))
        {
            if (maze_has_wall(x, y, dir))
            {
                continue;
            }

            int8_t dx = 0;
            int8_t dy = 0;
            maze_step_delta(dir, &dx, &dy);
            int8_t nx = (int8_t)(x + dx);
            int8_t ny = (int8_t)(y + dy);

            if (!maze_in_range(nx, ny))
            {
                continue;
            }

            int neighborIndex = maze_index(nx, ny);
            if (s_cells[neighborIndex].distance > nextDistance)
            {
                s_cells[neighborIndex].distance = nextDistance;
                queue[tail++] = (uint8_t)neighborIndex;
            }
        }
    }
}

/* DFS 模式下的选路策略：优先挑一个没走过且没墙的方向 */
static MazeDir maze_choose_unvisited_dir(void)
{
    MazeDir priority[4] = {MAZE_DIR_LEFT, MAZE_DIR_UP, MAZE_DIR_RIGHT, MAZE_DIR_DOWN};

    for (uint8_t i = 0u; i < 4u; ++i)
    {
        MazeDir targetDir = maze_relative_to_absolute(s_heading, priority[i]);
        int8_t dx = 0;
        int8_t dy = 0;
        maze_step_delta(targetDir, &dx, &dy);

        int8_t nx = (int8_t)(s_currentX + dx);
        int8_t ny = (int8_t)(s_currentY + dy);

        if (!maze_in_range(nx, ny))
        {
            continue;
        }

        if (maze_has_wall(s_currentX, s_currentY, targetDir))
        {
            continue;
        }

        if (!s_cells[maze_index(nx, ny)].visited)
        {
            return targetDir;
        }
    }

    return (MazeDir)0xFF;
}

/* Flood Fill 模式下的选路策略：优先走“距离更小”的方向 */
static MazeDir maze_choose_shortest_dir(void)
{
    MazeDir bestDir = (MazeDir)0xFF;
    uint8_t bestDistance = MAZE_INF_DISTANCE;
    MazeDir priority[4] = {MAZE_DIR_UP, MAZE_DIR_LEFT, MAZE_DIR_RIGHT, MAZE_DIR_DOWN};

    for (uint8_t i = 0u; i < 4u; ++i)
    {
        MazeDir targetDir = maze_relative_to_absolute(s_heading, priority[i]);
        int8_t dx = 0;
        int8_t dy = 0;
        maze_step_delta(targetDir, &dx, &dy);

        int8_t nx = (int8_t)(s_currentX + dx);
        int8_t ny = (int8_t)(s_currentY + dy);

        if (!maze_in_range(nx, ny))
        {
            continue;
        }

        if (maze_has_wall(s_currentX, s_currentY, targetDir))
        {
            continue;
        }

        uint8_t distance = s_cells[maze_index(nx, ny)].distance;
        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestDir = targetDir;
        }
    }

    return bestDir;
}

/* 单步主逻辑：
 * 1) 先把传感器信息写进地图
 * 2) 再根据当前算法决定下一步走向
 * 3) 最后执行电机动作并更新坐标
 */
static MazeAction maze_run_step(bool useFloodFill, float dis_front, float dis_left, float dis_right, bool gray_entered_next_cell)
{
    /* 如果已经结束，就不再继续走了 */
    if (s_finished)
    {
        TB6612_Brake();
        return MAZE_ACTION_FINISH;
    }

    /* 已经开始前进后，不再重新规划；直到灰度确认进入下一格 */
    if (s_waitingForNextCell)
    {
        TB6612_Forward(s_driveSpeed);

        if (!gray_entered_next_cell)
        {
            return MAZE_ACTION_FORWARD;
        }

        TB6612_Brake();
        s_waitingForNextCell = false;
        /* 如果还没进入迷宫内部，第一次灰度触发表示进入迷宫入口，坐标从 (0,2) 开始计数 */
        if (!s_insideMaze)
        {
            s_insideMaze = true;
            s_currentX = 0;
            s_currentY = 2;
            s_heading = s_pendingDir; /* 保证朝向与进格方向一致 */

            /* 把进入的这个格子记为已访问（DFS 模式） */
            maze_mark_current_visited();
            if (s_visitedCount >= MAZE_CELL_COUNT)
            {
                s_finished = true;
            }

            return s_finished ? MAZE_ACTION_FINISH : MAZE_ACTION_FORWARD;
        }

        /* 常规进格：提交坐标更新（可能发生出迷宫） */
        maze_commit_forward_step();

        /* 如果 commit 导致出迷宫（越界），会把 s_finished 置位并保证 s_heading 指向出口 */
        if (s_finished)
        {
            TB6612_Brake();
            return MAZE_ACTION_FINISH;
        }

        if (!useFloodFill)
        {
            maze_mark_current_visited();
            if (s_visitedCount >= MAZE_CELL_COUNT)
            {
                s_finished = true;
            }
        }
        else if (s_currentX == s_goalX && s_currentY == s_goalY)
        {
            s_finished = true;
        }

        return s_finished ? MAZE_ACTION_FINISH : MAZE_ACTION_FORWARD;
    }

    /* 先记录墙和已访问状态，再进行选路 */
    maze_update_walls_from_sensor(dis_front, dis_left, dis_right);
    maze_mark_current_visited();

    /* Flood Fill 每步都会重新计算距离场，保证新发现的墙能及时影响路径 */
    if (useFloodFill)
    {
        maze_rebuild_distance_field();
    }

    /* 到达目标格，Flood Fill 直接结束 */
    if (useFloodFill && s_currentX == s_goalX && s_currentY == s_goalY)
    {
        s_finished = true;
        TB6612_Brake();
        return MAZE_ACTION_FINISH;
    }

    /* 两种算法分别选下一步方向：DFS 偏探索，Flood Fill 偏最短路 */
    MazeDir nextDir = (MazeDir)0xFF;

    if (useFloodFill)
    {
        nextDir = maze_choose_shortest_dir();
    }
    else
    {
        nextDir = maze_choose_unvisited_dir();

        /* DFS 没有新路可走时，就尝试回退到路径栈中的上一个格子 */
        if ((uint8_t)nextDir == 0xFFu)
        {
            if (s_pathTop > 0u)
            {
                int8_t parentIndex = s_pathStack[s_pathTop - 1u];
                int8_t parentX = (int8_t)(parentIndex % MAZE_SIZE);
                int8_t parentY = (int8_t)(parentIndex / MAZE_SIZE);

                if ((parentX == s_currentX) && (parentY == s_currentY) && (s_pathTop > 1u))
                {
                    parentIndex = s_pathStack[s_pathTop - 2u];
                    parentX = (int8_t)(parentIndex % MAZE_SIZE);
                    parentY = (int8_t)(parentIndex / MAZE_SIZE);
                }

                if (maze_in_range(parentX, parentY))
                {
                    if (s_pathTop > 0u)
                    {
                        --s_pathTop;
                    }

                    if (parentX > s_currentX)
                    {
                        nextDir = MAZE_DIR_RIGHT;
                    }
                    else if (parentX < s_currentX)
                    {
                        nextDir = MAZE_DIR_LEFT;
                    }
                    else if (parentY > s_currentY)
                    {
                        nextDir = MAZE_DIR_DOWN;
                    }
                    else if (parentY < s_currentY)
                    {
                        nextDir = MAZE_DIR_UP;
                    }
                }
            }
        }
    }

    /* 没有可走方向时：要么已经遍历完，要么这一步先停下等待下一次调用 */
    if ((uint8_t)nextDir == 0xFFu)
    {
        if (s_visitedCount >= MAZE_CELL_COUNT)
        {
            s_finished = true;
            TB6612_Brake();
            return MAZE_ACTION_FINISH;
        }

        TB6612_Brake();
        return MAZE_ACTION_IDLE;
    }

    /* 记录动作类型，供上层显示/调试 */
    MazeAction action = maze_action_from_turn(nextDir);

    if (!useFloodFill)
    {
        /* DFS 里把当前格子压栈，回溯时可以找回上一个位置 */
        int8_t dx = 0;
        int8_t dy = 0;
        maze_step_delta(nextDir, &dx, &dy);
        int8_t nx = (int8_t)(s_currentX + dx);
        int8_t ny = (int8_t)(s_currentY + dy);

        if (maze_in_range(nx, ny))
        {
            if (s_pathTop < MAZE_CELL_COUNT)
            {
                s_pathStack[s_pathTop++] = (int8_t)maze_index(s_currentX, s_currentY);
            }
        }
    }

    /* 发起一次“转向 + 前进等待灰度进格”的动作 */
    maze_start_forward_wait_gray(nextDir);

    return action;
}

/* 重新开始一局迷宫：清空地图、重置坐标、重置参数 */
void Maze_Reset(void)
{
    memset(s_cells, 0, sizeof(s_cells));
    s_currentX = 0;
    s_currentY = 2;
    s_goalX = 3;
    s_goalY = 1;
    s_heading = MAZE_DIR_UP;
    s_algorithm = MAZE_ALGO_DFS;
    s_visitedCount = 0u;
    s_finished = false;
    s_wallThresholdMm = 220.0f;
    s_driveSpeed = 38.0f;
    s_waitingForNextCell = false;
    s_pendingDir = MAZE_DIR_UP;
    s_insideMaze = false;
    s_exitNumber = -1;
    s_pathTop = 0u;
    TB6612_Brake();
}

/* 设置起点位置和初始朝向 */
void Maze_SetStart(int8_t x, int8_t y, MazeDir heading)
{
    if (maze_in_range(x, y))
    {
        s_currentX = x;
        s_currentY = y;
    }

    s_heading = heading;
    s_pathTop = 0u;
    s_finished = false;
    s_waitingForNextCell = false;
}

/* 设置目标格子：Flood Fill 时会朝这里收敛 */
void Maze_SetGoal(int8_t x, int8_t y)
{
    if (maze_in_range(x, y))
    {
        s_goalX = x;
        s_goalY = y;
    }
}

/* 设置“多近算有墙”的阈值，具体值可以根据传感器实测再调 */
void Maze_SetWallThresholdMm(float thresholdMm)
{
    if (thresholdMm > 0.0f)
    {
        s_wallThresholdMm = thresholdMm;
    }
}

/* 设置电机速度和转向/前进的时间参数，便于你现场调车 */
void Maze_SetMoveProfile(float speed)
{
    if (speed > 0.0f)
    {
        s_driveSpeed = speed;
    }
}

/* 记录当前想用哪一种算法，上层可据此决定调用 DFS 还是 Flood Fill */
void Maze_SelectAlgorithm(MazeAlgorithm algorithm)
{
    s_algorithm = algorithm;
}

MazeAction Maze_DFS_Step(float dis_front, float dis_left, float dis_right, bool gray_entered_next_cell)
{
    s_algorithm = MAZE_ALGO_DFS;
    return maze_run_step(false, dis_front, dis_left, dis_right, gray_entered_next_cell);
}

MazeAction Maze_FloodFill_Step(float dis_front, float dis_left, float dis_right, bool gray_entered_next_cell)
{
    s_algorithm = MAZE_ALGO_FLOOD_FILL;
    return maze_run_step(true, dis_front, dis_left, dis_right, gray_entered_next_cell);
}

bool Maze_IsFinished(void)
{
    return s_finished;
}

uint8_t Maze_GetVisitedCount(void)
{
    return s_visitedCount;
}

int8_t Maze_GetCurrentX(void)
{
    return s_currentX;
}

int8_t Maze_GetCurrentY(void)
{
    return s_currentY;
}

MazeDir Maze_GetHeading(void)
{
    return s_heading;
}

/* 返回 1..4 表示最右侧四个出口（从上到下），-1 表示未出或非法出口 */
int8_t Maze_GetExitNumber(void)
{
    return s_exitNumber;
}