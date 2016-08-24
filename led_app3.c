/*
 * led_app3.c
 *
 */ 

#include <avr/io.h>
#include "rtetypes.h"
#include "rte_api.h"

void third_led_app_state_machine(uint8_t msgnum, uint32_t msgdata)
{
    static uint8_t led_flag = 0; /* By default, led is off */
    static uint32_t led_period;
    static uint8_t pwm_value;
    register             uint32_t *ptr;

    switch (msgnum)
    {
        case MSG_SYSTEM_INITIALISE:
            ptr = (uint32_t *)msgdata;
            ptr++;
            led_period = *ptr;
            ptr += 5;
            led_period += *ptr;
            break;
            
        case MSG_SYSTEM_TIMER_EXPIRATION:
            switch (msgdata)
            {
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    return;
}                