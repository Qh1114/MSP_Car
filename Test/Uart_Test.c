#include "ti_msp_dl_config.h"
#include "Uart_Test.h"
#include "Usart.h"
#include "Delay.h"
#include "Uart_Data.h"
void Uart_Test1(void)
{
    Uart0_Printf("Uart Test1\n");
    while(1)
    {
        Uart0_Printf("Time: %u\n", Get_ms());
    }
}

void Uart_Test2(void)
{
    Uart0_Printf("Uart Test2\n");
    while(1)
    {
        Uart0_Printf("Time: %u\n", (uint32_t)Get_us());
        Delay_ms(100);
    }
}

//extern uint8_t Buffer2[128];
void Uart_Test3(void)
{
    Uart0_Printf("Uart Test3\n");
    while(1)
    {
        uint8_t command[10];
        uint8_t length = UART_ReadCommand(command);
        if(length > 0){
            Uart0_Printf("Received Command: ");
            for(uint8_t i = 0; i < length; i++){
                Uart0_Printf("%02X ", command[i]);
            }
            Uart0_Printf("\n");
        }
        //if(Buffer2[0] != 0){
            //Uart0_Printf("Buffer2: %02X, %02X, %02X, %02X\n", Buffer2[0], Buffer2[1], Buffer2[2], Buffer2[3]);
        //}
        //Uart0_Printf("Count: %u\n", count);
        Delay_ms(5);
    }
}

void Uart_Test4(void)
{
    Uart0_Printf("Uart Test4\n");
    while(1)
    {
        float command[10];
        uint8_t length = Uart0_Receive_Command(command);
        if(length > 0){
            Uart0_Printf("Received Command: ");
            for(uint8_t i = 0; i < length; i++){
                Uart0_Printf("%f  ", command[i]);
            }
            Uart0_Printf("\n");
        }
    }
}