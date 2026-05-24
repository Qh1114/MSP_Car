/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2500
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for PWM_Motor */
#define PWM_Motor_INST                                                     TIMG6
#define PWM_Motor_INST_IRQHandler                               TIMG6_IRQHandler
#define PWM_Motor_INST_INT_IRQN                                 (TIMG6_INT_IRQn)
#define PWM_Motor_INST_CLK_FREQ                                         80000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_Motor_C0_PORT                                             GPIOB
#define GPIO_PWM_Motor_C0_PIN                                     DL_GPIO_PIN_26
#define GPIO_PWM_Motor_C0_IOMUX                                  (IOMUX_PINCM57)
#define GPIO_PWM_Motor_C0_IOMUX_FUNC                 IOMUX_PINCM57_PF_TIMG6_CCP0
#define GPIO_PWM_Motor_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_Motor_C1_PORT                                             GPIOB
#define GPIO_PWM_Motor_C1_PIN                                      DL_GPIO_PIN_3
#define GPIO_PWM_Motor_C1_IOMUX                                  (IOMUX_PINCM16)
#define GPIO_PWM_Motor_C1_IOMUX_FUNC                 IOMUX_PINCM16_PF_TIMG6_CCP1
#define GPIO_PWM_Motor_C1_IDX                                DL_TIMER_CC_1_INDEX

/* Defines for PWM_SERVO */
#define PWM_SERVO_INST                                                     TIMA0
#define PWM_SERVO_INST_IRQHandler                               TIMA0_IRQHandler
#define PWM_SERVO_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define PWM_SERVO_INST_CLK_FREQ                                         80000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_SERVO_C0_PORT                                             GPIOA
#define GPIO_PWM_SERVO_C0_PIN                                      DL_GPIO_PIN_0
#define GPIO_PWM_SERVO_C0_IOMUX                                   (IOMUX_PINCM1)
#define GPIO_PWM_SERVO_C0_IOMUX_FUNC                  IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWM_SERVO_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_SERVO_C1_PORT                                             GPIOA
#define GPIO_PWM_SERVO_C1_PIN                                      DL_GPIO_PIN_1
#define GPIO_PWM_SERVO_C1_IOMUX                                   (IOMUX_PINCM2)
#define GPIO_PWM_SERVO_C1_IOMUX_FUNC                  IOMUX_PINCM2_PF_TIMA0_CCP1
#define GPIO_PWM_SERVO_C1_IDX                                DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWM_SERVO_C3_PORT                                             GPIOA
#define GPIO_PWM_SERVO_C3_PIN                                     DL_GPIO_PIN_28
#define GPIO_PWM_SERVO_C3_IOMUX                                   (IOMUX_PINCM3)
#define GPIO_PWM_SERVO_C3_IOMUX_FUNC                  IOMUX_PINCM3_PF_TIMA0_CCP3
#define GPIO_PWM_SERVO_C3_IDX                                DL_TIMER_CC_3_INDEX

/* Defines for PWM_BUZZER */
#define PWM_BUZZER_INST                                                    TIMA1
#define PWM_BUZZER_INST_IRQHandler                              TIMA1_IRQHandler
#define PWM_BUZZER_INST_INT_IRQN                                (TIMA1_INT_IRQn)
#define PWM_BUZZER_INST_CLK_FREQ                                        80000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_BUZZER_C0_PORT                                            GPIOA
#define GPIO_PWM_BUZZER_C0_PIN                                    DL_GPIO_PIN_17
#define GPIO_PWM_BUZZER_C0_IOMUX                                 (IOMUX_PINCM39)
#define GPIO_PWM_BUZZER_C0_IOMUX_FUNC                IOMUX_PINCM39_PF_TIMA1_CCP0
#define GPIO_PWM_BUZZER_C0_IDX                               DL_TIMER_CC_0_INDEX



/* Defines for TIMER_10ms */
#define TIMER_10ms_INST                                                  (TIMG0)
#define TIMER_10ms_INST_IRQHandler                              TIMG0_IRQHandler
#define TIMER_10ms_INST_INT_IRQN                                (TIMG0_INT_IRQn)
#define TIMER_10ms_INST_LOAD_VALUE                                      (49999U)




/* Defines for I2C_0 */
#define I2C_0_INST                                                          I2C1
#define I2C_0_INST_IRQHandler                                    I2C1_IRQHandler
#define I2C_0_INST_INT_IRQN                                        I2C1_INT_IRQn
#define I2C_0_BUS_SPEED_HZ                                               1000000
#define GPIO_I2C_0_SDA_PORT                                                GPIOA
#define GPIO_I2C_0_SDA_PIN                                        DL_GPIO_PIN_16
#define GPIO_I2C_0_IOMUX_SDA                                     (IOMUX_PINCM38)
#define GPIO_I2C_0_IOMUX_SDA_FUNC                      IOMUX_PINCM38_PF_I2C1_SDA
#define GPIO_I2C_0_SCL_PORT                                                GPIOA
#define GPIO_I2C_0_SCL_PIN                                        DL_GPIO_PIN_15
#define GPIO_I2C_0_IOMUX_SCL                                     (IOMUX_PINCM37)
#define GPIO_I2C_0_IOMUX_SCL_FUNC                      IOMUX_PINCM37_PF_I2C1_SCL


/* Defines for UART */
#define UART_INST                                                          UART0
#define UART_INST_FREQUENCY                                             40000000
#define UART_INST_IRQHandler                                    UART0_IRQHandler
#define UART_INST_INT_IRQN                                        UART0_INT_IRQn
#define GPIO_UART_RX_PORT                                                  GPIOA
#define GPIO_UART_TX_PORT                                                  GPIOA
#define GPIO_UART_RX_PIN                                          DL_GPIO_PIN_11
#define GPIO_UART_TX_PIN                                          DL_GPIO_PIN_10
#define GPIO_UART_IOMUX_RX                                       (IOMUX_PINCM22)
#define GPIO_UART_IOMUX_TX                                       (IOMUX_PINCM21)
#define GPIO_UART_IOMUX_RX_FUNC                        IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_IOMUX_TX_FUNC                        IOMUX_PINCM21_PF_UART0_TX
#define UART_BAUD_RATE                                                  (115200)
#define UART_IBRD_40_MHZ_115200_BAUD                                        (21)
#define UART_FBRD_40_MHZ_115200_BAUD                                        (45)
/* Defines for UART_3 */
#define UART_3_INST                                                        UART3
#define UART_3_INST_FREQUENCY                                           80000000
#define UART_3_INST_IRQHandler                                  UART3_IRQHandler
#define UART_3_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_3_RX_PORT                                                GPIOA
#define GPIO_UART_3_TX_PORT                                                GPIOA
#define GPIO_UART_3_RX_PIN                                        DL_GPIO_PIN_25
#define GPIO_UART_3_TX_PIN                                        DL_GPIO_PIN_26
#define GPIO_UART_3_IOMUX_RX                                     (IOMUX_PINCM55)
#define GPIO_UART_3_IOMUX_TX                                     (IOMUX_PINCM59)
#define GPIO_UART_3_IOMUX_RX_FUNC                      IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART_3_IOMUX_TX_FUNC                      IOMUX_PINCM59_PF_UART3_TX
#define UART_3_BAUD_RATE                                                  (9600)
#define UART_3_IBRD_80_MHZ_9600_BAUD                                       (520)
#define UART_3_FBRD_80_MHZ_9600_BAUD                                        (53)
/* Defines for UART_WIRELESS */
#define UART_WIRELESS_INST                                                 UART1
#define UART_WIRELESS_INST_FREQUENCY                                    40000000
#define UART_WIRELESS_INST_IRQHandler                           UART1_IRQHandler
#define UART_WIRELESS_INST_INT_IRQN                               UART1_INT_IRQn
#define GPIO_UART_WIRELESS_RX_PORT                                         GPIOB
#define GPIO_UART_WIRELESS_TX_PORT                                         GPIOB
#define GPIO_UART_WIRELESS_RX_PIN                                  DL_GPIO_PIN_5
#define GPIO_UART_WIRELESS_TX_PIN                                  DL_GPIO_PIN_4
#define GPIO_UART_WIRELESS_IOMUX_RX                              (IOMUX_PINCM18)
#define GPIO_UART_WIRELESS_IOMUX_TX                              (IOMUX_PINCM17)
#define GPIO_UART_WIRELESS_IOMUX_RX_FUNC               IOMUX_PINCM18_PF_UART1_RX
#define GPIO_UART_WIRELESS_IOMUX_TX_FUNC               IOMUX_PINCM17_PF_UART1_TX
#define UART_WIRELESS_BAUD_RATE                                           (9600)
#define UART_WIRELESS_IBRD_40_MHZ_9600_BAUD                                (260)
#define UART_WIRELESS_FBRD_40_MHZ_9600_BAUD                                 (27)
/* Defines for UART_CAMERA */
#define UART_CAMERA_INST                                                   UART2
#define UART_CAMERA_INST_FREQUENCY                                      40000000
#define UART_CAMERA_INST_IRQHandler                             UART2_IRQHandler
#define UART_CAMERA_INST_INT_IRQN                                 UART2_INT_IRQn
#define GPIO_UART_CAMERA_RX_PORT                                           GPIOB
#define GPIO_UART_CAMERA_TX_PORT                                           GPIOB
#define GPIO_UART_CAMERA_RX_PIN                                   DL_GPIO_PIN_18
#define GPIO_UART_CAMERA_TX_PIN                                   DL_GPIO_PIN_17
#define GPIO_UART_CAMERA_IOMUX_RX                                (IOMUX_PINCM44)
#define GPIO_UART_CAMERA_IOMUX_TX                                (IOMUX_PINCM43)
#define GPIO_UART_CAMERA_IOMUX_RX_FUNC                 IOMUX_PINCM44_PF_UART2_RX
#define GPIO_UART_CAMERA_IOMUX_TX_FUNC                 IOMUX_PINCM43_PF_UART2_TX
#define UART_CAMERA_BAUD_RATE                                             (9600)
#define UART_CAMERA_IBRD_40_MHZ_9600_BAUD                                  (260)
#define UART_CAMERA_FBRD_40_MHZ_9600_BAUD                                   (27)




/* Defines for SPI_1 */
#define SPI_1_INST                                                         SPI1
#define SPI_1_INST_IRQHandler                                   SPI1_IRQHandler
#define SPI_1_INST_INT_IRQN                                       SPI1_INT_IRQn
#define GPIO_SPI_1_PICO_PORT                                              GPIOB
#define GPIO_SPI_1_PICO_PIN                                       DL_GPIO_PIN_8
#define GPIO_SPI_1_IOMUX_PICO                                   (IOMUX_PINCM25)
#define GPIO_SPI_1_IOMUX_PICO_FUNC                   IOMUX_PINCM25_PF_SPI1_PICO
#define GPIO_SPI_1_POCI_PORT                                              GPIOB
#define GPIO_SPI_1_POCI_PIN                                       DL_GPIO_PIN_7
#define GPIO_SPI_1_IOMUX_POCI                                   (IOMUX_PINCM24)
#define GPIO_SPI_1_IOMUX_POCI_FUNC                   IOMUX_PINCM24_PF_SPI1_POCI
/* GPIO configuration for SPI_1 */
#define GPIO_SPI_1_SCLK_PORT                                              GPIOB
#define GPIO_SPI_1_SCLK_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_1_IOMUX_SCLK                                   (IOMUX_PINCM26)
#define GPIO_SPI_1_IOMUX_SCLK_FUNC                   IOMUX_PINCM26_PF_SPI1_SCLK



/* Defines for BAT_ADC */
#define BAT_ADC_INST                                                        ADC0
#define BAT_ADC_INST_IRQHandler                                  ADC0_IRQHandler
#define BAT_ADC_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define BAT_ADC_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define BAT_ADC_ADCMEM_0_REF                     DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define BAT_ADC_ADCMEM_0_REF_VOLTAGE_V                                       3.3
#define GPIO_BAT_ADC_C0_PORT                                               GPIOA
#define GPIO_BAT_ADC_C0_PIN                                       DL_GPIO_PIN_27
#define GPIO_BAT_ADC_IOMUX_C0                                    (IOMUX_PINCM60)
#define GPIO_BAT_ADC_IOMUX_C0_FUNC                (IOMUX_PINCM60_PF_UNCONNECTED)



/* Port definition for Pin Group FLASH */
#define FLASH_PORT                                                       (GPIOB)

/* Defines for FLASH_CS: GPIOB.6 with pinCMx 23 on package pin 58 */
#define FLASH_FLASH_CS_PIN                                       (DL_GPIO_PIN_6)
#define FLASH_FLASH_CS_IOMUX                                     (IOMUX_PINCM23)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for LED_1: GPIOA.14 with pinCMx 36 on package pin 7 */
#define LED_LED_1_PIN                                           (DL_GPIO_PIN_14)
#define LED_LED_1_IOMUX                                          (IOMUX_PINCM36)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOB)

/* Defines for OLED_CS: GPIOB.14 with pinCMx 31 on package pin 2 */
#define OLED_OLED_CS_PIN                                        (DL_GPIO_PIN_14)
#define OLED_OLED_CS_IOMUX                                       (IOMUX_PINCM31)
/* Defines for OLED_DC: GPIOB.11 with pinCMx 28 on package pin 63 */
#define OLED_OLED_DC_PIN                                        (DL_GPIO_PIN_11)
#define OLED_OLED_DC_IOMUX                                       (IOMUX_PINCM28)
/* Defines for OLED_RES: GPIOB.10 with pinCMx 27 on package pin 62 */
#define OLED_OLED_RES_PIN                                       (DL_GPIO_PIN_10)
#define OLED_OLED_RES_IOMUX                                      (IOMUX_PINCM27)
/* Defines for ICM_CS: GPIOB.21 with pinCMx 49 on package pin 20 */
#define ICM_ICM_CS_PORT                                                  (GPIOB)
#define ICM_ICM_CS_PIN                                          (DL_GPIO_PIN_21)
#define ICM_ICM_CS_IOMUX                                         (IOMUX_PINCM49)
/* Defines for ICM_INT: GPIOA.30 with pinCMx 5 on package pin 37 */
#define ICM_ICM_INT_PORT                                                 (GPIOA)
#define ICM_ICM_INT_PIN                                         (DL_GPIO_PIN_30)
#define ICM_ICM_INT_IOMUX                                         (IOMUX_PINCM5)
/* Defines for MOTOR_AIN2: GPIOB.12 with pinCMx 29 on package pin 64 */
#define MOTOR_MOTOR_AIN2_PORT                                            (GPIOB)
#define MOTOR_MOTOR_AIN2_PIN                                    (DL_GPIO_PIN_12)
#define MOTOR_MOTOR_AIN2_IOMUX                                   (IOMUX_PINCM29)
/* Defines for MOTOR_AIN1: GPIOB.13 with pinCMx 30 on package pin 1 */
#define MOTOR_MOTOR_AIN1_PORT                                            (GPIOB)
#define MOTOR_MOTOR_AIN1_PIN                                    (DL_GPIO_PIN_13)
#define MOTOR_MOTOR_AIN1_IOMUX                                   (IOMUX_PINCM30)
/* Defines for MOTOR_STBY: GPIOA.13 with pinCMx 35 on package pin 6 */
#define MOTOR_MOTOR_STBY_PORT                                            (GPIOA)
#define MOTOR_MOTOR_STBY_PIN                                    (DL_GPIO_PIN_13)
#define MOTOR_MOTOR_STBY_IOMUX                                   (IOMUX_PINCM35)
/* Defines for MOTOR_BIN1: GPIOA.12 with pinCMx 34 on package pin 5 */
#define MOTOR_MOTOR_BIN1_PORT                                            (GPIOA)
#define MOTOR_MOTOR_BIN1_PIN                                    (DL_GPIO_PIN_12)
#define MOTOR_MOTOR_BIN1_IOMUX                                   (IOMUX_PINCM34)
/* Defines for PMOTOR_BIN2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define MOTOR_PMOTOR_BIN2_PORT                                           (GPIOB)
#define MOTOR_PMOTOR_BIN2_PIN                                   (DL_GPIO_PIN_23)
#define MOTOR_PMOTOR_BIN2_IOMUX                                  (IOMUX_PINCM51)
/* Defines for ENA_L: GPIOB.16 with pinCMx 33 on package pin 4 */
#define ENCODER_ENA_L_PORT                                               (GPIOB)
// groups represented: ["LASER","ENCODER"]
// pins affected: ["GPIO2","ENA_L","ENB_L","ENA_R"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODER_ENA_L_IIDX                                  (DL_GPIO_IIDX_DIO16)
#define ENCODER_ENA_L_PIN                                       (DL_GPIO_PIN_16)
#define ENCODER_ENA_L_IOMUX                                      (IOMUX_PINCM33)
/* Defines for ENB_L: GPIOB.2 with pinCMx 15 on package pin 50 */
#define ENCODER_ENB_L_PORT                                               (GPIOB)
#define ENCODER_ENB_L_IIDX                                   (DL_GPIO_IIDX_DIO2)
#define ENCODER_ENB_L_PIN                                        (DL_GPIO_PIN_2)
#define ENCODER_ENB_L_IOMUX                                      (IOMUX_PINCM15)
/* Defines for ENA_R: GPIOB.27 with pinCMx 58 on package pin 29 */
#define ENCODER_ENA_R_PORT                                               (GPIOB)
#define ENCODER_ENA_R_IIDX                                  (DL_GPIO_IIDX_DIO27)
#define ENCODER_ENA_R_PIN                                       (DL_GPIO_PIN_27)
#define ENCODER_ENA_R_IOMUX                                      (IOMUX_PINCM58)
/* Defines for ENB_R: GPIOA.29 with pinCMx 4 on package pin 36 */
#define ENCODER_ENB_R_PORT                                               (GPIOA)
// groups represented: ["LASER","ENCODER"]
// pins affected: ["GPIO1","GPIO3","ENB_R"]
#define GPIO_MULTIPLE_GPIOA_INT_IRQN                            (GPIOA_INT_IRQn)
#define GPIO_MULTIPLE_GPIOA_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define ENCODER_ENB_R_IIDX                                  (DL_GPIO_IIDX_DIO29)
#define ENCODER_ENB_R_PIN                                       (DL_GPIO_PIN_29)
#define ENCODER_ENB_R_IOMUX                                       (IOMUX_PINCM4)
/* Defines for OUT: GPIOB.25 with pinCMx 56 on package pin 27 */
#define GRAYSCALE_OUT_PORT                                               (GPIOB)
#define GRAYSCALE_OUT_PIN                                       (DL_GPIO_PIN_25)
#define GRAYSCALE_OUT_IOMUX                                      (IOMUX_PINCM56)
/* Defines for AD0: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GRAYSCALE_AD0_PORT                                               (GPIOB)
#define GRAYSCALE_AD0_PIN                                       (DL_GPIO_PIN_24)
#define GRAYSCALE_AD0_IOMUX                                      (IOMUX_PINCM52)
/* Defines for AD1: GPIOB.20 with pinCMx 48 on package pin 19 */
#define GRAYSCALE_AD1_PORT                                               (GPIOB)
#define GRAYSCALE_AD1_PIN                                       (DL_GPIO_PIN_20)
#define GRAYSCALE_AD1_IOMUX                                      (IOMUX_PINCM48)
/* Defines for AD2: GPIOA.22 with pinCMx 47 on package pin 18 */
#define GRAYSCALE_AD2_PORT                                               (GPIOA)
#define GRAYSCALE_AD2_PIN                                       (DL_GPIO_PIN_22)
#define GRAYSCALE_AD2_IOMUX                                      (IOMUX_PINCM47)
/* Defines for XSHUT1: GPIOA.24 with pinCMx 54 on package pin 25 */
#define LASER_XSHUT1_PORT                                                (GPIOA)
#define LASER_XSHUT1_PIN                                        (DL_GPIO_PIN_24)
#define LASER_XSHUT1_IOMUX                                       (IOMUX_PINCM54)
/* Defines for GPIO1: GPIOA.31 with pinCMx 6 on package pin 39 */
#define LASER_GPIO1_PORT                                                 (GPIOA)
#define LASER_GPIO1_IIDX                                    (DL_GPIO_IIDX_DIO31)
#define LASER_GPIO1_PIN                                         (DL_GPIO_PIN_31)
#define LASER_GPIO1_IOMUX                                         (IOMUX_PINCM6)
/* Defines for XSHUT2: GPIOB.22 with pinCMx 50 on package pin 21 */
#define LASER_XSHUT2_PORT                                                (GPIOB)
#define LASER_XSHUT2_PIN                                        (DL_GPIO_PIN_22)
#define LASER_XSHUT2_IOMUX                                       (IOMUX_PINCM50)
/* Defines for GPIO2: GPIOB.1 with pinCMx 13 on package pin 48 */
#define LASER_GPIO2_PORT                                                 (GPIOB)
#define LASER_GPIO2_IIDX                                     (DL_GPIO_IIDX_DIO1)
#define LASER_GPIO2_PIN                                          (DL_GPIO_PIN_1)
#define LASER_GPIO2_IOMUX                                        (IOMUX_PINCM13)
/* Defines for XSHUT3: GPIOB.0 with pinCMx 12 on package pin 47 */
#define LASER_XSHUT3_PORT                                                (GPIOB)
#define LASER_XSHUT3_PIN                                         (DL_GPIO_PIN_0)
#define LASER_XSHUT3_IOMUX                                       (IOMUX_PINCM12)
/* Defines for GPIO3: GPIOA.7 with pinCMx 14 on package pin 49 */
#define LASER_GPIO3_PORT                                                 (GPIOA)
#define LASER_GPIO3_IIDX                                     (DL_GPIO_IIDX_DIO7)
#define LASER_GPIO3_PIN                                          (DL_GPIO_PIN_7)
#define LASER_GPIO3_IOMUX                                        (IOMUX_PINCM14)
/* Defines for BUTTON1: GPIOA.8 with pinCMx 19 on package pin 54 */
#define BUTTON_BUTTON1_PORT                                              (GPIOA)
#define BUTTON_BUTTON1_PIN                                       (DL_GPIO_PIN_8)
#define BUTTON_BUTTON1_IOMUX                                     (IOMUX_PINCM19)
/* Defines for BUTTON2: GPIOA.9 with pinCMx 20 on package pin 55 */
#define BUTTON_BUTTON2_PORT                                              (GPIOA)
#define BUTTON_BUTTON2_PIN                                       (DL_GPIO_PIN_9)
#define BUTTON_BUTTON2_IOMUX                                     (IOMUX_PINCM20)
/* Defines for BUTTON3: GPIOB.15 with pinCMx 32 on package pin 3 */
#define BUTTON_BUTTON3_PORT                                              (GPIOB)
#define BUTTON_BUTTON3_PIN                                      (DL_GPIO_PIN_15)
#define BUTTON_BUTTON3_IOMUX                                     (IOMUX_PINCM32)
/* Defines for BUTTON4: GPIOB.19 with pinCMx 45 on package pin 16 */
#define BUTTON_BUTTON4_PORT                                              (GPIOB)
#define BUTTON_BUTTON4_PIN                                      (DL_GPIO_PIN_19)
#define BUTTON_BUTTON4_IOMUX                                     (IOMUX_PINCM45)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_PWM_Motor_init(void);
void SYSCFG_DL_PWM_SERVO_init(void);
void SYSCFG_DL_PWM_BUZZER_init(void);
void SYSCFG_DL_TIMER_10ms_init(void);
void SYSCFG_DL_I2C_0_init(void);
void SYSCFG_DL_UART_init(void);
void SYSCFG_DL_UART_3_init(void);
void SYSCFG_DL_UART_WIRELESS_init(void);
void SYSCFG_DL_UART_CAMERA_init(void);
void SYSCFG_DL_SPI_1_init(void);
void SYSCFG_DL_BAT_ADC_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
