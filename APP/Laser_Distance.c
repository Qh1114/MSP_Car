#include "Laser_Distance.h"
#include "my_vl53l1x.h"

void Laser_Distance_Init(void)
{
    my_vl53l1x_init();
}

int32_t Laser_Distance_Get(LaserDirection direction)
{
    int32_t distance;
    if (direction == LASER_LEFT)
    {
        my_vl53l1x_get_distance(&distance, LASER_LEFT);
        return distance;
    }else if (direction == LASER_FRONT)
    {
        my_vl53l1x_get_distance(&distance, LASER_FRONT);
        return distance;
    }else if (direction == LASER_RIGHT)
    {
        my_vl53l1x_get_distance(&distance, LASER_RIGHT);
        return distance;
    }else {
        return -1.0f; // Invalid direction
    }
}