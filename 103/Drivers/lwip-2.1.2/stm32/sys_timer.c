#include "main.h"

/**
 * @brief Override LWIP weak function
 */



extern uint32_t sys_rand()
{
    return SysTick->VAL;
}

extern uint32_t sys_get_tick_ms()
{
    return HAL_GetTick();
}



uint32_t sys_now(void)
{
    return sys_get_tick_ms();
}

/**
 * @brief Override LWIP weak function
 */
uint32_t sys_jiffies(void)
{
    return sys_get_tick_ms();
}

