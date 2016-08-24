/*
 * led_app1.c
 *
 */ 
#include <avr/io.h>
#include "rtetypes.h"
#include "rte_api.h"
void led_app_state_machine(uint8_t msgnum, uint32_t msgdata)
{
    uint8_t led_flag = 0; /* By default, led is off */
    switch (msgnum)
    {
        case MSG_SYSTEM_INITIALISE:
            /* Init message for process */
            
            /* Initialise PB3 and PB0 for output ports */
            DDRB |= (1 << DDB0);
            DDRB |= (1 << DDB3);
            PORTB ^= (1 << DDB0);
            PORTB ^= (1 << DDB3);
            
            /* Initialise timer */
            set_timer(1, 0x34, 100);
            set_timer(1, 0x69, 200);
            break;
            
         case MSG_SYSTEM_TIMER_EXPIRATION:
            switch (msgdata)
            {
                case 0x34:
                    set_timer(1, 0x34, 100);
                    PORTB ^= (1 << DDB0);
                    break;
                    
                case 0x69:
                    set_timer(1, 0x69, 200);
                    PORTB ^= (1 << DDB3);
                    break;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}