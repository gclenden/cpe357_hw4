#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "mytar.h"

int main(int argc, char** argv)
{
	uint8_t flags[6]; /*[c, t, x, v, f, S]*/
	int i;
	int ch;
	
	
	for(i=0; i<6; i++)
		flags[i]=0;

	if(argc>=3)
	{
	
		/*parse all of the give flags*/
		i=0;
		while((ch=argv[1][i++])!='\0')
		{
			printf("parsing flags -- ch: %c\n", ch);
			switch(ch)
			{
				case('c'):
					flags[0]=1;
					break;
				case('t'):
					flags[1]=1;
					break;
				case('x'):
					flags[2]=1;		
					break;		
				case('v'):
					flags[3]=1;	
					break;
				case('f'):
					flags[4]=1;
					break;
				case('S'):
					flags[5]=1;
					break;
				default:
					printUsage();
					return -1;
			}
		}

		/* f flag given -- update the output file*/
		if(flags[4] && argc == 4)
		{
			if((i=open(argv[2], O_WRONLY | O_TRUNC | O_CREAT))<0)
			{
				perror("open f");
				return -1;
			}

			else
			{
				dup2(i, STDOUT_FILENO);
				close(i);
			}
		}

		else if(!flags[4] && argc==3){;}

		else
		{
			printf("missing path\n");
			printUsage();
			close(STDOUT_FILENO);
			return -1;
		}

		
		/* see what function I'll need to callf from here*/
                if(flags[0] ^  flags[1] ^ flags[2])
                {
                        /*we know what function to do*/
			if(flags[0]);
			
			if(flags[1]);
	
			if(flags[2]);
				
                }

                else
                {
                        printUsage();
           
                }
	}
	else 
		printUsage();
	return 0;
	
}

void printUsage()
{
	printf("usage: mytar [ctxvS][f tarfile] [ path [ ... ] ]\n");
}
