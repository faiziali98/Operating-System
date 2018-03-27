// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
#define off 2

void IncrementPC()
{
  int pc = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, pc);
  pc = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, pc);
  pc += 4;
  machine->WriteRegister(NextPCReg, pc);
}

char* getString(int fid){
  char *filename = new char[100];
  //find a proper place to free this allocation
  int ch;
  if(!machine->ReadMem(fid,1,&ch))
    return NULL;
  int i=0;
  while( ch!=0 )
  {
    filename[i]=(char)ch;
    fid+=1;
    i++;
    if(!machine->ReadMem(fid,1,&ch))
      return NULL;
  }
  filename[i]=(char)0; 
  return filename;
}

void
ExceptionHandler(ExceptionType which)
{
    // printf("\nExceptionHandler was called.\n");
    int type = machine->ReadRegister(2); // system call code -- r2	
  	int arg1 = machine->ReadRegister(4); // arg1 -- r4
  	int arg2 = machine->ReadRegister(5); // arg2 -- r5
  	int arg3 = machine->ReadRegister(6); // arg3 -- r6
  	int arg4 = machine->ReadRegister(7); // arg4 -- r7
  	int num_bytes_read,ret_exec;
  	char* fileName;
  	char *buffer; 

  	if(which == SyscallException){

  		switch (type) {
    		case SC_Halt:
				DEBUG('a', "Shutdown, initiated by user program.\n");
				printf("Shutdown, initiated by user program.\n");
			   	interrupt->Halt();
			   	break;
			case SC_Exit:
				printf("User wants to exit.\n");
			 	ExitP(arg1);
			   	break;
			case SC_Fork:
				printf("Trying to fork a thread.\n");
			 	Fork(arg1);
			   break;
			case SC_Yield:
				printf("User wants to yield current thread.\n");
			 	Yield();
			   	break;
      case SC_Join:
        printf("User wants to join current thread.\n");
        Join(arg1);
        break;
			case SC_Exec:
		    	printf("User wants to execute another program.\n");
		    	ret_exec = Exec(getString(arg1));
          machine->WriteRegister(2, ret_exec);
		    	break;
			case SC_Create:
				fileName = getString(arg1);
		    	printf("User wants to create a file named %s.\n",fileName);
		    	Create(fileName);
		    	break;
	  		case SC_Open: 
	  			fileName = getString(arg1);
		    	printf("User wants to open a file named %s.\n",fileName);
		    	Open(fileName);
		    	break;
	    	case SC_Write:
		    	buffer = getString(arg1);
		    	Write(buffer,arg2,arg3);
		    	break;
	    	case SC_Read:
		    	buffer = new char[100];
		    	num_bytes_read = Read(buffer,arg2,arg3);
		    	strcpy(&machine->mainMemory[arg1], buffer);
				  machine->WriteRegister(2, num_bytes_read);
		    	break;
	    	case SC_Close:
		    	printf("User wants to close a file.\n");
		    	Closef(arg1);
		    	break;
	    	default:
				printf("Unexpected user mode exception %d %d\n", which, type);
				//ASSERT(FALSE);
				break;
	    }
	}

	IncrementPC();
}

void 
Create(char *name){
	if (name == NULL) {
      printf("Could not find file name in memory.\n");
    }
    if (strlen(name) == 0) {
      printf("File name had a length of 0.\n");
    }
      
    printf("Creating a file %s\n", name);
    
    bool result = fileSystem->Create(name, 100);
    if (result)
    {
     printf("File %s was successfully created\n", name);
    } else {
     printf("error creating file %s\n", name);
    }
}

OpenFileId 
Open(char *name){
	if (currentThread->space->fileCounter + off>= 100)
    {
      printf("Too many files open.\n");
      return -1;
    }
    if (name == NULL)
    {
      printf("Could not find file name string reference in memory.\n");
      return -1;
    }

    if (strlen(name) == 0)
    {
      printf("Filename had a length of zero.\n");
      return -1;
    }

    printf("Opening a file %s\n", name);   

    OpenFile* file = fileSystem->Open(name);
    if (file == NULL)
    {
      printf("File system could not find the file specified.\n");
      return -1;
    }
    printf("File opened successfully.\n");

    currentThread->space->openFileTable[currentThread->space->fileCounter + off] = file;
    int t = currentThread->space->fileCounter + off;
    machine->WriteRegister(2, currentThread->space->fileCounter + off);
    currentThread->space->fileCounter++;

    return t;
}

void 
Closef(OpenFileId id){
	if (currentThread->space->openFileTable[id] == NULL) {
      printf("Could not find an open file with that id...\n");
    }
    printf("Closed a file successfully\n");
    delete currentThread->space->openFileTable[id];
    currentThread->space->fileCounter--;
}

void 
Write(char *buffer, int size, OpenFileId id){
  	if (buffer == NULL)
  	{
     	printf("Cannot find string to write.\n");
  	}
  	if (id == ConsoleOutput)
  	{
  		printf("User wants to write to the console.\n");
    	printf("%s\n",buffer);
  	} 
  	else if (id == ConsoleInput)
    {
      	printf("Cannot write to console input.\n");
    } 
  	else
  	{
  		printf("Attempting to write %s to file %d\n", buffer, id);
    	OpenFile* file = currentThread->space->openFileTable[id];
    	if (file == NULL) {
       		printf("File does not exist!\n");
    	}
    	file->Write(buffer, size + 1);
  	}
}

int 
Read(char *buffer, int size, OpenFileId id)
{
	if (size == 0){
	    printf("Cannot read zero bytes.\n");
	    return -1;
	}
	  
	if (id == ConsoleInput){
	  	printf("User wants to read from the console.\n");
	    scanf("%s", buffer);
		if (strlen(buffer) == 0) 
			printf("Bad pointer passed to Read: data not copied\n");
	}else if (id == ConsoleOutput){
	    printf("Cannot read from console output.\n");
	    return -1;
	}else {
	  	printf("Attempting to read %d bytes from file %d\n", size, id);
	    OpenFile* file = currentThread->space->openFileTable[id];
	    if (file == NULL) {
	      printf("File does not exist!\n");
	      return -1;
	    }
	    file->Read(buffer, size + 1);
	}

	int newlen;
	newlen = strlen(buffer);

	printf("Read string %s with len %d\n", buffer, newlen);
	return newlen;
}

void 
ExitP(int status){
	if(status==0)
		currentThread->Finish();
	else
		printf("The program didn't exit normally.\n");
}

void to_fork(int alpha) {
	currentThread -> space -> InitRegisters();
	currentThread -> space -> RestoreState();
	machine -> Run();
	ASSERT(FALSE);
} 

SpaceId 
Exec(char *name)
{
	printf("\n\nINSIDE EXEC\n\n%s\n\n", name);
	OpenFile *executable = fileSystem->Open(name);
  AddrSpace *space;

  if (executable == NULL) {
		printf("Unable to open file %s\n", name);
    return 0;
  }
    
  space = new AddrSpace(executable);  
  Thread *t = new Thread("thread",true);
  t->space = space;  
  t->parent = currentThread;
  currentThread->children[currentThread->no_of_child]=t;
  int k = currentThread->no_of_child;
  currentThread->no_of_child++;

  printf("forking the thread.\n");
  t -> Fork(to_fork,0);
  return k;
}

void ForkUserThread(int funcPtr)
{
  DEBUG('t', "Setting machine PC to funcPtr for thread %s: 0x%x...\n", currentThread->getName(), funcPtr);
 
  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();
  
  // update the stack register
  machine->WriteRegister(StackReg, currentThread->space->GetNumPages() * PageSize - 16);

  // Set the program counter to the appropriate place indicated by funcPtr...
  machine->WriteRegister(PCReg, funcPtr);
  machine->WriteRegister(NextPCReg, funcPtr + 4);
  machine->Run();
}

void 
Fork(int ptr)
{
  printf("Forking\n");
  char* tname = new char[100];
  tname="new_t";
  Thread* thread = new Thread(tname,1);
  thread->space = currentThread->space;

  currentThread->children[currentThread->no_of_child]=thread;
  int k = currentThread->no_of_child;
  currentThread->no_of_child++;
  printf("%d\n", k);
  machine->WriteRegister(2, k);
  thread->Fork(ForkUserThread,ptr);
}

void 
Yield()
{
  printf("Thread yielding %s\n", currentThread->getName());
  currentThread->Yield();
}

void
Join(SpaceId id){
  if (id<currentThread->no_of_child){
    (currentThread->children[id])->Join();
  }else{
    return;
  }
}