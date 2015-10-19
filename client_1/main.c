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
	initAll();
	wdt_enable(WDTO_120MS);
	RunTasks();
    while (1) 
    {
      wdt_reset();	// Сброс собачьего таймера
      TaskManager();	// Вызов диспетчера
    }
}

