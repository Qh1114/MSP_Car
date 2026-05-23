#include "OLED_Test.h"
#include "OLED.h"
void OLED_Test1(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, "Hello, OLED!", 16, 0);
    OLED_Refresh();
    while(1) {
    }
}