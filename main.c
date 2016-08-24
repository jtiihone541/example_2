/*
 * main.c
 */ 

#include <avr/io.h>
#include "rte.h"
#include "led_app1.h"
#include "led_app2.h"
#include "led_app3.h"

int main(void)
{
    
    /* Replace with your application code */
    init_system();
    add_process(led_app_state_machine);
    add_process(second_led_app_state_machine);
    add_process(third_led_app_state_machine);
    configure_system();
    while (1) 
    {
        schedule_loop();
    }
}

