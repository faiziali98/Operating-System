#ifndef ALARM_CC
#define ALARM_CC

#include "copyright.h"
#include "alarmclock.h"
#include "system.h"

// void wakeUp() {
// 	if(!(a -> threadList -> IsEmpty())) {
// 		Thread *temp = (Thread*) (a -> threadList -> Remove());
// 		if(temp -> waketime <= stats -> totalTicks) {
// 			IntStatus oldlevel = interrupt -> SetLevel(IntOff);
// 			scheduler -> ReadyToRun(temp);
// 			printf("Wake up called after %d ticks \n", temp -> waketime);
// 			(void) interrupt -> SetLevel(oldlevel);
// 		} else {
// 			a -> threadList -> SortedInsert(temp, temp -> waketime);
// 		}
// 	} else {
//         printf("no thread waiting \n");
//     }
// }

Alarm::Alarm() 
{
    //initialize list
	threadList = new List;
    // timer = new timer(wakeUp, 1, 0);
}

Alarm::~Alarm() 
{
	delete timer;
	delete threadList;
}

void Alarm::GoToSleepFor(int howLong) 
{
	Thread* temp = currentThread;
	temp -> waketime = stats -> totalTicks + howLong;
	
	threadList -> SortedInsert(temp, temp -> waketime);
	IntStatus oldlevel = interrupt -> SetLevel(IntOff);
	currentThread -> Sleep();
	(void) interrupt -> SetLevel(oldlevel);
}

#endif
