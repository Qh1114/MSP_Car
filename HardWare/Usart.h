#ifndef __USART_H__
#define __USART_H__

#include "ti_msp_dl_config.h"
void Uart_Init(void);
void Uart0_Send_Char(char ch);
void Uart0_Send_String(char* str);
void Uart0_Printf(const char* format, ...);
uint8_t UART_ReadCommand(uint8_t* command);


#endif
