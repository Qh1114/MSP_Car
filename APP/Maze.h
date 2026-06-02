#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* 小车当前朝向：上下左右 4 个方向*/
    // 顺序是为了方便计算相对方向（左转/右转/掉头）
    typedef enum
    {
        MAZE_DIR_UP = 0,
        MAZE_DIR_RIGHT = 1,
        MAZE_DIR_DOWN = 2,
        MAZE_DIR_LEFT = 3
    } MazeDir;

    /* 算法返回给电机的动作指令 */
    typedef enum
    {
        MAZE_ACTION_IDLE = 0,
        MAZE_ACTION_FORWARD,
        MAZE_ACTION_TURN_LEFT,
        MAZE_ACTION_TURN_RIGHT,
        MAZE_ACTION_TURN_BACK,
        MAZE_ACTION_FINISH
    } MazeAction;

    typedef enum // 两种算法：DFS 和 Flood Fill
    {
        MAZE_ALGO_DFS = 0,
        MAZE_ALGO_FLOOD_FILL = 1
    } MazeAlgorithm;

/* 赛题地图固定为 4x4，因此这里直接写死 */
#define MAZE_SIZE 4
#define MAZE_CELL_COUNT (MAZE_SIZE * MAZE_SIZE)

    /* 初始化API */
    void Maze_Reset(void);
    void Maze_SetStart(int8_t x, int8_t y, MazeDir heading);
    void Maze_SetGoal(int8_t x, int8_t y);
    void Maze_SetWallThresholdMm(float thresholdMm);
    void Maze_SetMoveProfile(float speed);
    void Maze_SelectAlgorithm(MazeAlgorithm algorithm);

    /* DFS：用于“尽量遍历 16 个区块”
     * gray_entered_next_cell: 前置灰度传感器进格标志（true 表示已经跨入下一格）
     * 输入三路距离值，内部会判断前/左/右是否有墙，并决定下一步走向。
     */
    MazeAction Maze_DFS_Step(float dis_front, float dis_left, float dis_right, bool gray_entered_next_cell);

    /* Flood Fill：用于“最短路径/快速冲线”
     * gray_entered_next_cell: 前置灰度传感器进格标志（true 表示已经跨入下一格）
     * 适合已知障碍后，快速朝目标点收敛。
     */
    MazeAction Maze_FloodFill_Step(float dis_front, float dis_left, float dis_right, bool gray_entered_next_cell);

    /* 下面几个接口主要用于调试和显示状态 */
    bool Maze_IsFinished(void);
    uint8_t Maze_GetVisitedCount(void);
    int8_t Maze_GetCurrentX(void);
    int8_t Maze_GetCurrentY(void);
    MazeDir Maze_GetHeading(void);
    /* 如果从出口出场，返回 1..4 表示最右侧四个出口（从上到下），未出或非合法出口返回 -1 */
    int8_t Maze_GetExitNumber(void);

#ifdef __cplusplus
}
#endif

#endif

/*最简单的具体运用
extern float ypr[3];  // IMU 姿态角 [Roll, Yaw, Pitch]，ypr[1] 是航向角

int main(void)
{

    HAL_Init();
    SystemClock_Config();  // 系统时钟
    Delay_Init();          // 延时
    TB6612_Init();         // 电机驱动初始化
    IMU_Init();            // IMU 姿态初始化
    Sensor_Init();         // 红外测距、灰度传感器初始化

    // ====================== 迷宫模块初始化（核心）======================
    Maze_Reset();                          // 【必调】重置迷宫所有状态（坐标、墙、栈、标志位）
    Maze_SetStart(0, 2, MAZE_DIR_RIGHT);   // 设置小车起点 (X=0,Y=2)，初始朝向 右（和代码默认一致）
    Maze_SetGoal(3, 1);                    // 设置FloodFill目标点 (3,1)
    Maze_SetWallThresholdMm(220.0f);       // 设置墙判定阈值（mm，和代码默认一致，现场可调）
    Maze_SetMoveProfile(38.0f);            // 设置电机基础速度

    // 选择算法（二选一）
    Maze_SelectAlgorithm(MAZE_ALGO_DFS);         // 模式1：DFS 全迷宫探路
    Maze_SelectAlgorithm(MAZE_ALGO_FLOOD_FILL);  // 模式2：FloodFill 走最短路径到目标

    float dis_front, dis_left, dis_right; // 三路红外距离
    bool gray_flag = false;               // 灰度进格标志
    MazeAction act;                       // 算法返回动作

    // 2. 主循环（迷宫持续运行）
    while(1)
    {
        // -------- 步骤1：实时采样所有传感器（时序关键）--------
        // 刷新IMU航向角，否则转弯卡死
        // 读取前/左/右距离(mm)
        // 读取灰度：是否检测到格线（驶入下一格）

        // -------- 步骤2：调用迷宫单步算法（核心接口）--------
        // 根据选择的算法调用对应接口，二选一
        act = Maze_DFS_Step(dis_front, dis_left, dis_right, gray_flag);
        act = Maze_FloodFill_Step(dis_front, dis_left, dis_right, gray_flag);

        // -------- 步骤3：可选：调试打印（串口输出动作、坐标、出口号）--------
        // UART_SendData("Act:%d X:%d Y:%d\r\n", act, Maze_GetCurrentX(), Maze_GetCurrentY());

        // -------- 步骤4：判断任务是否结束 --------
        if(Maze_IsFinished() == true)
        {
            TB6612_Brake();  // 停车
            int8_t exit = Maze_GetExitNumber();
        }
    }
}
*/