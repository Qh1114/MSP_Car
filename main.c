/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "Menu.h"



#include "Usart.h"
#include "spi.h"
#include "Key.h"
#include "Grayscale.h"


#include "Uart_Test.h"
#include "vl53l1x_Test.h"
#include "ICM_Test.h"
#include "OLED_Test.h"
#include "ADC_Test.h"
#include "Encoder_Test.h"
#include "TB6612_Test.h"
#include "Motor_Test.h"
#include "Key_Test.h"
#include "Drive_Test.h"
#include "Grayscale_Test.h"
#include "Servo_Test.h"
int main(void)
{
    SYSCFG_DL_init();

    Menu_Pre();
    
    Uart_Init();
    SPI_Init();
    Key_Init();
    NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_10ms_INST);
    
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);

    NVIC_EnableIRQ(TIMER_2ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_2ms_INST);
    //Uart_Test2();
    //Encoder_Test3();
    //Encoder_Test2();
    // VL53L1X_Test2();
    //   Servo_Test();
    //ICM42688_Test5();    
    // Grayscale_Test1();
    // Grayscale_Test2();
    // // 
    //Motor_Test3();
    //TB6612_Test2();
    //Motor_Test2();
    Drive_Test3();
    // Motor_Test2();
    // TB6612_Test1(); 
    
    // //Drive_Test1();
    //Drive_Test2();
    // Key_Test();

    // //
    // //Motor_Test1();
    // //Uart_Test3();
 
    // ICM42688_Test4();
    // ICM42688_Test3();   
    // //ICM42688_Test1();
   
    // ICM42688_Test2();
    
    // 
    // Encoder_Test1();
    // ADC_Test2();
    // ADC_Test1();
    
    
    // OLED_Test1();
    
    // VL53L1X_Test1();
    // //Uart_Test1();
    // Uart_Test2();
    while (1) {
        
    }
}

