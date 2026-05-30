#include "Servo.h"
#include "ti_msp_dl_config.h"


void Servo_SetPosition(ServoId servo_id, float duty)
{
    if (duty > MAX_DUTY)
    {
        duty = MAX_DUTY;
    }
    if (duty < MIN_DUTY)
    {
        duty = MIN_DUTY;
    }

    switch (servo_id)
    {
    case SERVO_ID_1:
        DL_TimerA_setCaptureCompareValue(SERVO_INST, SERVO_MAX_CCR_VALUE * duty / 100.0f, SERVO1_CCR_IDX);
        break;
    case SERVO_ID_2:
        DL_TimerA_setCaptureCompareValue(SERVO_INST, SERVO_MAX_CCR_VALUE * duty / 100.0f, SERVO2_CCR_IDX);
        break;
    case SERVO_ID_3:
        DL_TimerA_setCaptureCompareValue(SERVO_INST, SERVO_MAX_CCR_VALUE * duty / 100.0f, SERVO3_CCR_IDX);
        break;
    default:
        /* Invalid servo_id, do nothing or handle error */
        break;
    }
}
