#include "ti_msp_dl_config.h"
#include "Delay.h"

uint32_t MS = (uint32_t)(CPUCLK_FREQ / 1000);
uint32_t US = (uint32_t)(CPUCLK_FREQ / 1000000);
volatile static uint32_t now_tick = 0;

void Delay_ms(uint32_t delay_time)
{
    while(delay_time --)
        delay_cycles(MS);
}

void Delay_us(uint32_t delay_time)
{
    while(delay_time --)
        delay_cycles(US);
}

//获取现在时间
uint64_t Get_ms(void)
{
    return now_tick;
}

uint64_t Get_us(void)
{
    uint32_t ms1, ms2;
    uint32_t val;
    uint32_t reload = SysTick->LOAD; /* SysTick reload value */
    uint32_t ticks_per_us = US; /* CPU ticks per microsecond */

    /* Loop until we get a stable snapshot of now_tick and SysTick->VAL */
    do {
        ms1 = now_tick;
        val = SysTick->VAL;
        ms2 = now_tick;
    } while (ms1 != ms2);

    /* VAL counts down from LOAD to 0 */
    uint32_t elapsed_ticks = reload - val;

    uint32_t us_in_ms = elapsed_ticks / ticks_per_us;

    return (uint64_t)ms1 * 1000u + us_in_ms;
}

//系统计时
void SysTick_Handler(void)
{
    now_tick++;
}