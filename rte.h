void add_process(void(*process_msg_handler)(uint8_t msgnum, uint32_t msgdata));
void init_system(void);
void configure_system(void);
void schedule_loop(void);
