#include "Servo_Test.h"
#include "Servo.h"
#include "Delay.h"

void Servo_Test(void)
{
    // Servo_SetPosition(SERVO_ID_1, 12.5);
    // Servo_SetPosition(SERVO_ID_2, 12.5);
    // Servo_SetPosition(SERVO_ID_3, 12.5);
    while(1) {
        Servo_SetPosition(SERVO_ID_1, 12.5);
        Servo_SetPosition(SERVO_ID_2, 12.5);
        Servo_SetPosition(SERVO_ID_3, 12.5);
        Delay_ms(2000);
        Servo_SetPosition(SERVO_ID_1, 5.0);
        Servo_SetPosition(SERVO_ID_2, 5.0);
        Servo_SetPosition(SERVO_ID_3, 5.0);
        Delay_ms(2000);
        // Servo_SetPosition(SERVO_ID_1, 2.5);
        // Servo_SetPosition(SERVO_ID_2, 2.5);
        // Servo_SetPosition(SERVO_ID_3, 2.5);
        // Delay_ms(2000);

    }
}