
/*
 * tasks.c
 *
 * Created: 10/16/2015 4:45:26 PM
 *  Author: vaterlet
 */ 
#include "defines.h"

void TaskLedOn (void)
{
	SetTimerTask(TaskLedOn,10);
	PORTB = 0x05;
}