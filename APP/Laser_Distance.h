#ifndef __LASER_DISTANCE_H_
#define __LASER_DISTANCE_H_

#include "ti_msp_dl_config.h"
typedef enum
{
    LASER_LEFT = 1,
    LASER_FRONT = 2,
    LASER_RIGHT = 3
} LaserDirection;

void Laser_Distance_Init(void);
int32_t Laser_Distance_Get(LaserDirection direction);

#endif
