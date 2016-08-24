/*
 * led_app2.c
 *
 */ 
#include <avr/io.h>
#include "rtetypes.h"
#include "rte_api.h"
void second_led_app_state_machine(uint8_t msgnum, uint32_t msgdata)
{
    static uint8_t led_flag = 0; /* By default, led is off */
    static uint32_t led_period;
    static uint8_t pwm_value;
    switch (msgnum)
    {
        case MSG_SYSTEM_INITIALISE:
            /* Init message for process */
            
            /* Initialise PB2 for output port */
            DDRB |= (1 << DDB2);
            PORTB ^= (1 << DDB2);
            led_period = 1;
            led_flag = 0;
            /* Initialise timer */
            set_timer(2, 0x11, 10);
            
            /* Initialise timer1 & PB1 for PWM-mode */
            DDRB |= (1 << PB1);
            
            TCCR1 |= ( 1 << PWM1A);
            TCCR1 ^= (1 << COM1A1);
            TCCR1 |= (1 << CTC1);
            TCCR1 |= (1 << COM1A0);
            OCR1C = 255;
            OCR1A = 1;
            TCNT1 = 0;
            TCCR1 |= 0x8;
            
            GTCCR |= (1 << FOC1A);
            pwm_value = 1;

            break;
            
         case MSG_SYSTEM_TIMER_EXPIRATION:
            switch (msgdata)
            {
                case 0x11:
                    set_timer(2, 0x11, led_period);
                    PORTB ^= (1 << DDB2);
                    if (led_flag == 1)
                    {
                        /* Led is now off, new on period with updated timer value */
                        led_flag = 0;
                        led_period++;
                        if (led_period > 20)
                        {
                            led_period = 1;
                            
                        }
                    }
                    else
                    {
                        led_flag = 1;
                    }
                    pwm_value++;
                    if (pwm_value == 255)
                    {
                        pwm_value = 1;
                    }
                    OCR1A = pwm_value;
                    break;
                    

                    
                default:
                    break;
            }
            break;
        
        case 0x22:
            /* Message sent by itself */
            led_flag = 0;
            break;            
        default:
            break;
    }
}