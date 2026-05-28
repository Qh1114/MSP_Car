#ifndef __DRIVE_H
#define __DRIVE_H

void Drive_Init(void);
void Drive_Straight(float speed);
void Drive_Turn(float speed, float turn_rate);
void Drive_Stop(void);

#endif // __DRIVE_H