#include "copyright.h"
#include "synch.h"
#include "system.h"


class port{
	public:
		int buffer;
		Lock read_write("l");
		Condition send('s');
		Condition receiver('r');
};
port prt;
void sendHelper (int msg){
	send (prt,msg);
}
void send(port p, int msg){
	
	p.send.wait(p.read_write);
	p.buffer=p;
	p.send.signal(p.read_write);
	printf("Waiting for receiver to read");
	p.receiver.wait(p.read_write);
	printf("Allah Hafiz");
}

void receive(port p){
	p.send.signal(p.read_write);
	p.send.wait(p.read_write);
	printf("*** You received %d\n ***", p.buffer);
	p.receiver.signal(p.read_write);
}

int main(){
    Thread *t = new Thread("Sender thread");

    t->Fork(sendHelper,3);
    Thread *t1 = new Thread("Receiver thread");

    t1->Fork(receive,prt);
}