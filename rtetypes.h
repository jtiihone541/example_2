#define MSG_SYSTEM_TIMER_EXPIRATION 1
#define MSG_SYSTEM_INITIALISE 2

typedef struct
{
    uint8_t process;
    uint8_t msgnumber;
    uint8_t dummy1;
    uint8_t dummy2;
    uint32_t msgdata;
} message_t;

typedef struct
{
    uint8_t process;
    uint8_t timer_id;
    uint32_t expiration_time;
} timer_entry_t;

typedef struct
{
    uint8_t process_id;
    void(*process_msg_handler)(uint8_t msgnum, uint32_t msgdata);
} process_entry_t;

typedef struct
{
    uint8_t process_id;
    
} process_status_t;