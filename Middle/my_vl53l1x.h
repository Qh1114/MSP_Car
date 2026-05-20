#ifndef __MY_VL53L1X_H__
#define __MY_VL53L1X_H__

#include "ti_msp_dl_config.h"

void my_vl53l1x_init(void);
void my_vl53l1x_get_distance(int32_t *distance_out, int VL53L1X_ID);
void my_vl53l1x_callback(int VL53L1X_ID);


#endif