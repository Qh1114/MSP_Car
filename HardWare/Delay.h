#ifndef __DELAY_H__
#define __DELAY_H__

#define PERIODIC_R(T) static uint32_t lastTick = 0; \
                    if (Get_ms() - lastTick < T) \
                    { \
                        return; \
                    } \
                    lastTick += T;
#define PERIODIC_C(T) static uint32_t lastTick = 0; \
                    if (Get_ms() - lastTick < T) \
                    { \
                        continue; \
                    } \
                    lastTick = Get_ms(); \

void Delay_ms(uint32_t delay_time);
void Delay_us(uint32_t delay_time);
uint64_t Get_ms(void);
uint64_t Get_us(void);

#endif
