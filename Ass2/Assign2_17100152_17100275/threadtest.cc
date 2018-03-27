// threadtest.cc 
//  Simple test case for the threads assignment.
//
//  Create two threads, and have them context switch
//  back and forth between themselves by calling Thread::Yield, 
//  to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//#include "alarm.h"
#include "copyright.h"
#include "system.h"
#include "synch.h"
// testnum is set in main.cc
#include "utility.h"
#include "list.h"
#include "thread.h"
#include "system.h"


int testnum = 1;

class Passenger{
  public:
    int start;
    int dest;
    Passenger(int s, int d){
      start = s;
      dest = d;
    }
};

class Train{
  public:
    int currSta;
    int people;
    List *trn;
    Train(){
      people = 0;
      currSta = 0;
      trn = new List;
    }
    
    void insert(Passenger *p){
      Thread* temp;
      temp = currentThread;
      temp->waketime = p->dest;
      trn->SortedInsert(temp,temp->waketime);
      IntStatus oldlevel = interrupt->SetLevel(IntOff);
      currentThread->Sleep();
      (void) interrupt->SetLevel(oldlevel);
    }
    void wakeUp(){
      int i=1;
      while (i<=people){
        if (!(Thread*)trn->IsEmpty()) {
          Thread *temp=(Thread*)trn->Remove();
            if (temp->waketime == currSta) {
                printf("Train is at %d\n", currSta);
                IntStatus oldlevel = interrupt->SetLevel(IntOff);
                scheduler->ReadyToRun(temp);
                (void) interrupt->SetLevel(oldlevel);
                people--;
                printf("Remaining people %d\n", people);
                currentThread->Yield();
            }else{
              trn->Append(temp);
            }
        }else{
          break;
        }
        i++;
      }
    }
};

class Platform{
  public:
    Train *currTr;
    Semaphore *st;
    int waiting;
    Platform(){
      waiting = 0;
      st = new Semaphore("S",0);
    }
};

Platform *p = new Platform[5];

void pThread(Platform &pl, int s, int d){
  Passenger *ps = new Passenger(s,d);
  printf("I am at platform waiting for the train \n");
  pl.waiting = pl.waiting + 1;
  pl.st->P();
  printf("I got on the train %d %d\n",pl.currTr->currSta,d);
  pl.currTr->insert(ps);
  printf("Reached the destination\n");
}

void tThread(int k){
  Train *tr = new Train;
  printf("Staring the train \n");
  while (1){
    tr->wakeUp();
    p[tr->currSta].currTr = tr;
    while (p[tr->currSta].waiting>0 && tr->people<=50){
      p[tr->currSta].st->V();
      p[tr->currSta].waiting = p[tr->currSta].waiting - 1;
      tr->people = tr->people + 1;
    }
    currentThread->Yield();
    tr->currSta = tr->currSta + 1;
    if (tr->currSta>=5) tr->currSta = 0;

  }
}

void pthelp(int d){
  pThread(p[4],4,d);
}
void pthelp1(int d){
  pThread(p[1],1,d);
}
void metro(){
  Thread *temp1 = new Thread("forked thread two");
  temp1->Fork(tThread, 3);
  Thread *temp = new Thread("forked thread");
  temp->Fork(pthelp, 2);
  Thread *temp2 = new Thread("forked thread");
  temp2->Fork(pthelp, 3);
  Thread *temp3 = new Thread("forked thread");
  temp3->Fork(pthelp1, 0);
  
}

class Alarm{
  public:
    Alarm();
    ~Alarm();
   void GoToSleepFor(int howLong);
   List *threadlist; 
    Timer *timer;
};

Alarm::Alarm()
{
    threadlist = new List;
}

Alarm::~Alarm() { 
    delete timer; 
}

void Alarm::GoToSleepFor(int howLong) {
    Thread* temp;
    temp = currentThread;
    temp->waketime = stats->totalTicks + howLong;
    printf("current ticks = %d\n", stats->totalTicks);
    threadlist->SortedInsert(temp,temp->waketime);

    IntStatus oldlevel = interrupt->SetLevel(IntOff);
    currentThread->Sleep();
    (void) interrupt->SetLevel(oldlevel);
}

Alarm *a = new Alarm;

void CallBack(int l) 
{ 
    if (!(Thread*)(a->threadlist->IsEmpty())) {
      Thread *temp=(Thread*)(a->threadlist->Remove());
        if (temp->waketime <= stats->totalTicks) {
            IntStatus oldlevel = interrupt->SetLevel(IntOff);
            scheduler->ReadyToRun(temp);
            printf("awaked the thread waiting for %d ticks\n", temp->waketime);
            (void) interrupt->SetLevel(oldlevel);
        }else{
          a->threadlist->SortedInsert(temp,temp->waketime);
        }
    }

}

void print(int k){
  printf("Hello\n");
  printf("How are you\n");
  printf("Ach ok fir\n");
  printf("Me going to sleep\n");
  printf("sleeping for %d\n", k);
  a->GoToSleepFor(k);
  printf("Hello agian\n");
  printf("How are you\n");
  printf("Ach ok fir\n");
  printf("ab me chala\n");
}

void testAlarm(){
    a->timer = new Timer(CallBack,1,0);
    printf("Alarm thread created\n");
    Thread *t = new Thread("Alarm thread");
    t->Fork(print,200);
    Thread *t1 = new Thread("Alarm threaasd");
    t1->Fork(print,200);
    Thread *t2 = new Thread("Alarm threadsad");
    t2->Fork(print,200);
}

void
Joiner(Thread *joinee)
{
  currentThread->Yield();
  currentThread->Yield();

  printf("Waiting for the Joinee to finish executing.\n");

  currentThread->Yield();
  currentThread->Yield();

  joinee->Join();

  currentThread->Yield();
  currentThread->Yield();

  printf("Joinee has finished executing, we can continue.\n");

  currentThread->Yield();
  currentThread->Yield();
}

void
Joinee()
{
  int i;

  for (i = 0; i < 5; i++) {
    printf("Smell the roses.\n");
    currentThread->Yield();
  }

  currentThread->Yield();
  printf("Done smelling the roses!\n");
  currentThread->Yield();
}

void
ForkerThread()
{
  Thread *joiner = new Thread("joiner", 0);  // will not be joined
  Thread *joinee = new Thread("joinee", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Joinee, 0);

  // this thread is done and can go on its merry way
  printf("Forked off the joiner and joiner threads.\n");
}

class port{
    public:
        int buffer;
        int receivers;
        int sender;
        Lock *read_write;
        Condition *send;
        Condition *receiver;
        Condition *waiting;
        port (){
            buffer=0;
            sender=0;
            receiver=0;
            read_write=new Lock("l");
            send=new Condition("s");
            receiver=new Condition("r");
            waiting=new Condition("w");
        }
};

void yieldhelp(){
  Thread *te = currentThread;
  te->Yield();
}
port *prt=new port();
void send(port *p, int msg, int n){
     printf("%d Sender thread created\n", n);
    p->read_write->Acquire();
    p->sender = p->sender + 1;
    if (p->sender>1){
      printf("%d Waiting for previous transactions to end\n", n);
      p->waiting->Wait(p->read_write);
    } 
    if (p->receivers==0){
      printf("%d Waiting for receiver to come\n", n);
      p->send->Wait(p->read_write);
    }
    printf("%d Receiver found Writing..\n", n);
    p->buffer=msg;
    printf("%d Waiting for receiver to read\n", n);
    p->receiver->Signal(p->read_write);
    p->send->Wait(p->read_write);
    printf("%d Allah Hafiz\n", n);
    p->sender = p->sender - 1;
    if (p->sender>0) {p->waiting->Signal(p->read_write);}
    printf("%d All done \n", n);
    p->read_write->Release();
}

void receive(port *p, int n){
    p->receivers = p->receivers + 1;
     printf("Receiver came\n");
    p->read_write->Acquire();
    if (p->sender!=0) p->send->Signal(p->read_write);
    printf("%d am waiting now\n", n);
    p->receiver->Wait(p->read_write);
    printf("***%d received %d***\n",n, p->buffer);
    p->send->Signal(p->read_write);
    p->read_write->Release();
    p->receivers = p->receivers - 1;
    printf("Reader left\n");
}

int k=0;
void sendHelper (int msg){
    k++;
    send (prt,msg,k);
}
void rHelper (int msg){
    receive (prt,msg);
}
void abc(){

  Thread *t = new Thread("1");
  Thread *t1 = new Thread("2");
  Thread *t2 = new Thread("3");
  Thread *t3 = new Thread("4");
  Thread *t4 = new Thread("5");
  Thread *t5 = new Thread("6");

    t3->Fork(rHelper,2);
    currentThread->Yield();

    t->Fork(sendHelper,3);    
    currentThread->Yield();
    //printf("Sender thread created\n");
    t2->Fork(sendHelper,5);
    currentThread->Yield();

    t1->Fork(rHelper,1);
    currentThread->Yield();
    //t3->Fork(rHelper,2);

    t4->Fork(sendHelper,8);
    currentThread->Yield();
    t5->Fork(rHelper,3);
    // printf("Receiver thread created\n");

    //t3->Fork(rHelper,2);
}


//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    // for (num = 0; num < 5; num++) {
  printf("*** thread %d looped %d times\n", which, num);
    //     currentThread->Yield();
    // }
}
//----------------------------------------------------------------------
// ThreadTest1
//  Set up a ping-pong between two threads, by forking a thread 
//  to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");
    Thread *t1 = new Thread("forked thread");
    Thread *t2 = new Thread("forked thread");
    Thread *t3 = new Thread("forked thread");
    Thread *t4 = new Thread("forked thread");
    t->Fork(SimpleThread, 1);
    t1->Fork(SimpleThread, 2);
    t2->Fork(SimpleThread, 3);
    t3->Fork(SimpleThread, 4);
    t4->Fork(SimpleThread, 5);
}


//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
  metro();
  break;
    default:
  printf("No test specified.\n");
  break;
    }
}

