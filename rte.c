
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include "rtetypes.h"
#include "led_app1.h"
#include "led_app2.h"

#define NUM_OF_MESSAGES 5
message_t messages[NUM_OF_MESSAGES];
volatile uint8_t msg_write;
volatile uint8_t msg_read;

#define NUM_OF_PROCESSES 2
uint8_t process_table_entry=0;
process_entry_t process_entry[NUM_OF_PROCESSES];

#define NUM_OF_TIMERS 4
timer_entry_t timer_entry[NUM_OF_TIMERS];

uint32_t system_tick;
ISR(TIMER0_COMPA_vect)
{
    uint8_t i;
    system_tick++;
    for (i = 0; i < NUM_OF_TIMERS; i++)
    {
        if (timer_entry[i].process == 0) continue;
        if (timer_entry[i].expiration_time == system_tick)
        {
            /* This timer entry is expiring now, create a timer msg */
            messages[msg_write].process = timer_entry[i].process;
            messages[msg_write].msgnumber = MSG_SYSTEM_TIMER_EXPIRATION;
            messages[msg_write].msgdata = timer_entry[i].timer_id;
            msg_write++;
            if (msg_write == NUM_OF_MESSAGES)
            {
                msg_write = 0;
            }
            timer_entry[i].process = 0;
        }
        } /* timers are handled */

    }
    void await_interrupt(void)
    {
        sleep_cpu();
        return;
    }

uint8_t get_process_table_id(uint8_t process_id)
{
    uint8_t i = 0;
    
    for (i = 0; i < NUM_OF_PROCESSES; i++)
    {
        if (process_entry[i].process_id == process_id)
        {
            
            return i;
        }
    }
    return 0xff;
}

void add_process(void(*process_msg_handler)(uint8_t msgnum, uint32_t msgdata))
{
    /* Adds process "main function" into process table and assigns a process_id to it */
    if (process_table_entry == NUM_OF_PROCESSES)
    {
        /* Table full, not added... */
        return;
    }
    process_entry[process_table_entry].process_id = process_table_entry + 1;
    process_entry[process_table_entry].process_msg_handler = process_msg_handler;
    process_table_entry++;
    return;
}

void init_system(void)
{
    system_tick = 0;
    memset(process_entry, 0, sizeof(process_entry));
    memset(timer_entry, 0, sizeof(timer_entry));
    memset(messages, 0, sizeof(messages));
    messages[0].dummy1 = 1;
    messages[0].dummy2 = 1;
    msg_write = 0;
    msg_read = 0;
}
void configure_system(void)
{
    uint8_t i;
    /* Init system clock to 8MHz, assume default fuses are correct... */
    asm volatile 
    (
        "push R16\n"
        "ldi R16, 128\n"
        "out 0x26, R16\n"
        "ldi R16,0\n"
        "out 0x26, R16\n"
        "pop R16"
    );
    //CLKPR = 128;
    //CLKPR = 0x0;
    
    /* Initialise timer0 for timer interrupts */
    GTCCR |= (1 << TSM);
    TCCR0A |= 1 << WGM01;
    TCCR0B |= 1 << CS02;
    TCCR0B |= 1 << CS00;
    TCNT0 = 0;
    OCR0A = 78; /* Approx 100Hz system clock */
    TIMSK |= 1 << OCIE0A;
    GTCCR ^= (1 << TSM);
    
    /* Configure sleep mode to be idle, and allow also use of sleep mode */
    MCUCR &= 0xfc;
    MCUCR |= (1 << SE);
    
   
    for (i = 0; i < NUM_OF_PROCESSES; i++)
    {
        if (process_entry[i].process_id != 0)
        {
            process_entry[i].process_msg_handler(MSG_SYSTEM_INITIALISE, (uint32_t)&process_entry[i]);
        }            
    }
    /* Enable interrupts */
    SREG |= 128;
}    
    
    void set_timer(uint8_t process, uint8_t timer_id, uint32_t expiration_time)
    {
        uint8_t i;
        for (i = 0; i < NUM_OF_TIMERS; i++)
        {
            if (0 == timer_entry[i].process)
            {
                /* First free timer entry found, use it */
                timer_entry[i].process = process;
                timer_entry[i].timer_id = timer_id;
                timer_entry[i].expiration_time = expiration_time + system_tick;
                return;
            }
        }
        /* No free entry found, should return an error*/
    }

    void cancel_timer(uint8_t timer_id)
    {
        uint8_t i;
        for (i = 0; i < NUM_OF_TIMERS; i++)
        {
            if (timer_entry[i].timer_id == timer_id)
            {
                /* Entry found, clear it */
                timer_entry[i].process = 0;

            }
        }
    }
 
 void send_message(uint8_t target_process, uint8_t msg_number, uint32_t msg_data)   
 {
     /* Note: this must be executed with interrupts disabled, systick interrupt may othervise interrupt this */
     register message_t *msg;
     cli();
     msg = &messages;
     msg += msg_write;
     msg -> process = target_process;
     msg -> msgnumber = msg_number;
     msg -> msgdata = msg_data;

     msg_write++;
     if (msg_write == NUM_OF_MESSAGES)
     {
         msg_write = 0;
     }
     sei();
 }     
    void schedule_loop(void)
    {
        int msg_target = 0;
        uint8_t num;
        uint32_t data;

        while (1)
        {
            /* Check for possible message */
            if (msg_write != msg_read)
            {
                /* New message has been arrived, send it to target process */
                msg_target = get_process_table_id(messages[msg_read].process);
                num = messages[msg_read].msgnumber;
                data = messages[msg_read].msgdata;
                process_entry[msg_target].process_msg_handler(num, data);
                data = messages[msg_read].msgdata;

                msg_read++;
                if (msg_read == NUM_OF_MESSAGES)
                {
                    msg_read = 0;
                }
            }
            /* Previous loop may have generated new messages, which must be handled immediately */
            /* So sleep will be entered only if no messages awaiting... */
            if (msg_write == msg_read)
            {
                await_interrupt();
            }
        }
    }