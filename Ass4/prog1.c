#include "syscall.h"

int
main()
{
  int *arr[5000];
  int i;

  char *buff = "I eat memory\n";

  for (i = 0; i < 5000; i++) {
  	 arr[i] = 28;
  }

  Write(buff, 12, ConsoleOutput);
  
	Write("\nexiting program...\n\n", 50, ConsoleOutput);
	Exit(1);
}
