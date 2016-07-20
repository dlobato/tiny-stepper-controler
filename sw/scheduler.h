/* Name: scheduler.h
* modified from: https://sites.google.com/site/avrtutorials2/scheduler
* Project: tiny-stepper-controller
* Author: David Lobato <dav.lobato@gmail.com>
* Creation Date: 2016-07-01
* Copyright: (c) 2016 David Lobato
* License: GPLv3 ?
*/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <inttypes.h>

// configured for F_CPU 1MHz (8Mhz clock , div 8)
// 0 = STOP (Timer not counting)
// 1 = CLOCK      tics= 1MHz
// 2 = CLOCK/8    tics= 125kHz
// 3 = CLOCK/64   tics= 15625Hz
// 4 = CLOCK/256  tics= 1953.125Hz
// 5 = CLOCK/1024 tics= 244.141Hz
#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024


#define TIMER0_PRESCALER TIMER_CLK_DIV64
#define TIMER0_PRESCALER_FACTOR 64
#define SYSTICK_DELAY_MS 10
#define TIMER0_COMPARE_COUNT 155 //( SYSTICK_DELAY_MS*10e-3 * (F_CPU/TIMER0_PRESCALER_FACTOR) ) - 1 (TIMER0_COMPARE_COUNT < 255)


#define MAX_TASKS 8

enum task_status_t{
	RUNNABLE,
	RUNNING,
	STOPPED,
	ERROR,
};

enum task_rc_t{
	RC_CONTINUE,
	RC_DONE,
	RC_ERROR
};

// a task "type"
// pointer to a void function with no arguments
typedef enum task_rc_t (*task_t)(void);

// basic task control block (TCB)
struct tcb_t{
	//uint8_t id; // task ID
	task_t task; // pointer to the task
	// delay before execution
	uint16_t delay, period;
	enum task_status_t status; // status of task
};


// scheduler functions
void init_scheduler(void);
uint8_t add_task(task_t task, uint16_t period);//period in 1e10-2 seconds units
void delete_task(uint8_t id);
enum task_status_t get_task_status(uint8_t id);
void dispatch_tasks(void);
uint32_t millis(void);


#endif //SCHEDULER_H
