/*
 * TaskLamp.h
 *
 * Created: 10/20/2015 17:36:07 PM
 *  Author: vaterlet
 */ 


#ifndef TASKLAMP_H_
#define TASKLAMP_H_

#include "HAL.h"
#include "EERTOS.h"

void TaskSideLightsTurnOff (void);
void TaskSideLightsTurnOn (void);
int16_t GetTimeToOff();

#endif /* TASKLAMP_H_ */