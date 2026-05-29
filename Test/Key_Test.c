#include "Key_Test.h"
#include "Usart.h"
#include "Key.h"

void Key_Test(void)
{
   // Key_Init();
    uint8_t key_num = 0;
    while(1)
    {
        key_num = Key_Num_Get();
        if(key_num != 0)
        {
            Uart0_Printf("Key %d pressed\r\n", key_num);
        }
    }
}