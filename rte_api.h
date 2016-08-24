/*
 * rte_api.h
 */ 


#ifndef RTE_API_H_
#define RTE_API_H_

void set_timer(uint8_t process, uint8_t timer_id, uint32_t expiration_time);
void cancel_timer(uint8_t timer_id);



#endif /* RTE_API_H_ */