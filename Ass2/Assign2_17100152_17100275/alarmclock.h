#ifndef ALARM_H
#define ALARM_H

#include "copyright.h"
#include "timer.h"
#include "list.h"
#include "scheduler.h"

class Alarm 
{
	public:
		Alarm();
		~Alarm();
		void GoToSleepFor(int howLong);
		List *threadList;
		Timer *timer;
        int ticks;
};

#endif
