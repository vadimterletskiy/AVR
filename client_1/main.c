/*
 * GccApplication1.c
 *
 * Created: 10/16/2015 3:50:29 PM
 * Author : vaterlet
 */ 

#include "defines.h"

int main(void)
{
	InitRTOS();
	RunRTOS();
	wdt_enable(WDTO_120MS);
	SetTask(TaskLedOn);
    while (1) 
    {
      wdt_reset();	// Сброс собачьего таймера
      TaskManager();	// Вызов диспетчера
    }
}

