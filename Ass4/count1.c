#include "syscall.h"


int isEqual(char a[], char b[])
{
   int c = 0;
 
   while (a[c] == b[c]) {
      if (a[c] == '\0' || b[c] == '\0')
         break;
      c++;
   }
 
   if (a[c] == '\0' && b[c] == '\0')
      return 0;
   else
      return -1;
}


int
main()
{

	OpenFileId f;
	int i;
	int cond;
	char input[50];
	char result[10];
	int number;

	cond = 1;
	number = 0;
	f = Open("../test/CS370WordList.csv");

	Write("\n\nEnter the word which you want to search in file: ", 50, ConsoleOutput);
	Read(input, 50, ConsoleInput);


	while(cond){
		char word[50];
		i = 0;
		do{
		    Read(&word[i], 1, f); 

		    if(word[i] == '.'){
		    	cond = 0;
		    	break;
		    }

		} while(word[i++] != ' ');

		word[--i] = '\0';

		if(isEqual(input,word) != -1)
			number++;
	}

	result[0] = number + '0';
	
	Write("This word occured ", 18, ConsoleOutput);
	Write(result,1,ConsoleOutput);
	Write(" times in the file\n\n",7,ConsoleOutput);

}
