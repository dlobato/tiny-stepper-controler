/* Name: scheduler.c
* Project: twostates_lamp
* modified from: https://sites.google.com/site/avrtutorials2/scheduler
* Creation Date: 2015-02-25
*/

#include "scheduler.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

// the task list
struct tcb_t task_list[MAX_TASKS];

//millis since init_scheduler
volatile uint32_t timer0_millis = 0;

// scheduler function definitions

// initialises the task list
void init_scheduler(void){
	//init task list
	for(uint8_t i=0; i<MAX_TASKS; i++){
		//task_list[i].id = 0;
		task_list[i].task = (task_t)0x00;
		task_list[i].delay = 0;
		task_list[i].period = 0;
		task_list[i].status = STOPPED;
	}

	//init timer0
	/*
	Control Register A for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
	TCCR0A is 8 bits: [COM0A1:COM0A0:COM0B1:COM0B0:unused:unused:WGM01:WGM00]
	sets bits WGM0[1:0]=2, CTC mode
	*/
	TCCR0A = _BV(WGM01);

	/*
	Control Register B for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
	TCCR0B is 8 bits: [FOC0A:FOC0B:unused:unused:WGM02:CS02:CS01:CS00]
	CS0[2:0] timer prescaler
	*/
	TCCR0B = TIMER0_PRESCALER << CS00;

	OCR0A = TIMER0_COMPARE_COUNT;
	TCNT0 = 0;

	TIMSK |= _BV(OCIE0A);
}

// adds a new task to the task list
// scans through the list and
// places the new task data where
// it finds free space
uint8_t add_task(task_t task, uint16_t period){
	uint8_t idx = 0;
	while( idx < MAX_TASKS ){
		if( task_list[idx].status == STOPPED ){
			//task_list[idx].id = id;
			task_list[idx].task = task;
			task_list[idx].delay = period;
			task_list[idx].period = period;
			task_list[idx].status = RUNNABLE;
			break;
		}
		idx++;
	}
	return idx;
}

// remove task from task list
// note STOPPED is equivalent
// to removing a task
void delete_task(uint8_t id){
	if (id >= 0 || id < MAX_TASKS){
		task_list[id].status = STOPPED;
	}
}

// gets the task status
// returns ERROR if id is invalid
enum task_status_t get_task_status(uint8_t id){
	if (id >= 0 || id < MAX_TASKS){
		return task_list[id].status;
	}else{
		return ERROR;
	}
}

// dispatches tasks when they are ready to run
void dispatch_tasks(void){
	for(uint8_t i=0; i<MAX_TASKS; i++){
		// check for a valid task ready to run
		if( !task_list[i].delay && task_list[i].status == RUNNABLE ){
			// task is now running
			task_list[i].status = RUNNING;
			// call the task
			enum task_rc_t rc = (*task_list[i].task)();

			switch (rc){
			case RC_CONTINUE:
				// reset the delay
				task_list[i].delay = task_list[i].period;
				// task is runnable again
				task_list[i].status = RUNNABLE;
				break;
			case RC_DONE:
				task_list[i].status = STOPPED;
				break;
			case RC_ERROR:
				task_list[i].status = ERROR;
				break;
			}
		}
	}
}

uint32_t millis(){
	uint32_t m;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		m = timer0_millis;
	}
	return m;
}

//ISR_NOBLOCK won't block other interrupts, this way we still can have high priority interrupts
ISR(TIMER0_COMPA_vect,ISR_NOBLOCK)
{
	timer0_millis += SYSTICK_DELAY_MS;
	// cycle through available tasks
	for(uint8_t i=0;i<MAX_TASKS;i++){
		if( task_list[i].status == RUNNABLE ){
			task_list[i].delay--;
		}
	}
}
