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

	OpenFileId f;
	int i;
	OpenFileId f1;
	OpenFileId f2;
	int cond;
	int clen;
	int lengths[50];
	char input[50];
	char result[10];
	char totalwords[10][50];
	int number;
	int len;
	int notfound;
	int inloop;
	int tra;
	int count;

	notfound=1;
	cond = 1;
	clen=-1;
	number = 0;
	inloop=0;
	len=0;
	tra=0;
	count=0;
	Create("../test/result.csv");
	f = Open("../test/textdoc.csv");
	f2 = Open("../test/result.csv");

	while(cond){
		char word[50];
		i = 0;
		do{
		    Read(&word[i], 1, f); 
		    clen++;
		    if(word[i] == '.'){
		    	notfound=0;
		    	cond = 0;
		    	break;
		    }

		} while(word[i++] != ' ');
		word[--i] = '\0';

		if (len==0){
			strCpy(word,totalwords[0]);
			lengths[0] = clen;
			len++;
		}else{
			while (inloop<len){
				if(isEqual(totalwords[inloop],word) != -1){
					notfound=0;
					break;
				}
				inloop++;
			}
			if (notfound==1){
				strCpy(word,totalwords[len]);
				lengths[len] = clen;
				len++;
			}
			notfound=1;
			inloop=0;
		}
		clen = -1;
	}

	result[0] = len + '0';
	Write(result,1,ConsoleOutput);
	cond = 1;

	while(inloop<len){
		Write(totalwords[inloop],50,ConsoleOutput);
		f1 = Open("../test/textdoc.csv");
		while(cond){
			char word[50];
			i = 0;
			do{
			    Read(&word[i], 1, f1); 

			    if(word[i] == '.'){
			    	cond = 0;
			    	break;
			    }

			} while(word[i++] != ' ');

			word[--i] = '\0';

			if(isEqual(totalwords[inloop],word) != -1)
				number++;
		}
		Write(totalwords[inloop],lengths[inloop],f2);
		Write(": ",2,f2);
		result[0] = number + '0';
		Write(result,1,f2);
		Write("\n",1,f2);
		inloop++;
		cond=1;
		number=0;
	}	

}
