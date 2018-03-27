#include "syscall.h"

void strCpy(char str1[],char str2[]){
    int i=0;
    while(str1[i]!='\0'){
        str2[i] = str1[i];
        i++;
    }

    str2[i]='\0';
}


int
main()
{

	OpenFileId f,f2;
	int i,j;
	int temp;
	int clen;
	int forl;
	int in;
	int cond;
	char result[10],t[50];
	char strings[10][50];
	int lengths[50];
	int string_index;
	
	cond = 1;
	clen=-1;
	in=0;
	forl=0;
	Create("../test/Sorted-CS370Students.csv");
	f = Open("../test/CS370Students.csv");
	f2 = Open("../test/Sorted-CS370Students.csv");
	string_index = 0;



	while(cond){
		char word[50];
		i = 0;

		do{
		    Read(&word[i], 1, f); 
		    clen++;

		    if(word[i] != '.'){}
		    else{
		    	cond = 0;
		    	break;
		    }

		} while(word[i++] != ' ');

		word[--i] = '\0';

		if (cond){
			strCpy(word,strings[string_index]);
			lengths[string_index] = clen;
		}
		clen = -1;
		string_index++;
	}
	
	for (i = 0; i < string_index; i++) {
      	for (j = 0; j < string_index-i-1; j++) {

      		if (lengths[j - 1]>lengths[j]) forl = lengths[j-1];
      		else forl = lengths[j];

      		for (in=0;in<forl;in++){
	        	if (strings[j - 1][in] > strings[j][in]) {
		            strCpy(strings[j - 1], t);
		            Write(t,50,ConsoleOutput);
		            Write(strings[j],50,ConsoleOutput);
		            strCpy(strings[j], strings[j-1]);
		            strCpy(t,strings[j]);

		            temp = lengths[j-1];
		            lengths[j-1] = lengths[j];
		            lengths[j] = temp;
		            
		            break;
	        	}else if (strings[j - 1][in] < strings[j][in]){
	        		break;
	        	}
	        }
      }
   	}

   	Write("\nSorted Strings: ", 50, ConsoleOutput);
   	for(i = 0;i < string_index;i++){
   		Write(strings[i],50,ConsoleOutput);
   		result[0] = lengths[i] + '0';
   		Write(result,50,ConsoleOutput);
   		Write(strings[i],lengths[i],f2);
   		Write("\n",1,f2);
   	}
}
