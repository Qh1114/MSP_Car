#ifndef __USART_H__
#define __USART_H__

void Uart_Init(void);
void Uart0_Send_Char(char ch);
void Uart0_Send_String(char* str);
void Uart0_Printf(const char* format, ...);


#endif
