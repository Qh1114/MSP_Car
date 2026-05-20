#include "ti_msp_dl_config.h"
#include "Usart.h"
#include "Delay.h"
#include "string.h"
#include "stdarg.h"
#include <stdio.h>

void Uart_Init(void)
{
    NVIC_ClearPendingIRQ(UART_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_INST);
    NVIC_EnableIRQ(UART_INST_INT_IRQN);
}

//串口发送单个字符
void Uart0_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_INST) == true);
    DL_UART_Main_transmitData(UART_INST, ch);
}

//串口发送字符串
void Uart0_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart0_Send_Char(*str++);
    }
}

//串口打印
void Uart0_Printf(const char* format, ...)
{
    char tmp[128];  
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart0_Send_String(tmp);
}


void UART_0_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_INST) )
    {
        case DL_UART_IIDX_RX:
            break;

        default:
            break;
    }
}

