#ifndef EERTOS_H
#define EERTOS_H
//from lib

extern void InitRTOS(void);
extern void RunRTOS (void);
//extern void Idle(void);

typedef void (*TPTR)(void);

extern void SetTask(TPTR TS);
extern void SetTimerTask(TPTR TS, uint16_t NewTime);

extern void TaskManager(void);
//extern void TimerService(void);

//RTOS Errors Пока не используются.
//#define TaskSetOk			 'A'
//#define TaskQueueOverflow	 'B'
//#define TimerUpdated		 'C'
//#define TimerSetOk			 'D'
//#define TimerOverflow		 'E'

#endif
