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
	int pathindex;
	int file=STDOUT_FILENO;
	
	
	for(i=0; i<6; i++)
		flags[i]=0;

	if(argc>=3)
	{
	
		/*parse all of the give flags*/
		i=0;
		while((ch=argv[1][i++])!='\0')
		{
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
		if(flags[4] && argc >= 4)
		{
			if((file=open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0666))<0)
			{
				perror("open f");
				return -1;
			}

			else

			pathindex=3;
		}

		else if(!flags[4] && argc>=3)
		{
			pathindex=2;
		}

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
			if(flags[0])
			{
				while(pathindex<argc)
				{
					if(createArchive(file, argv[pathindex], flags[3], flags[5])<0)
					{
						close(file);
						perror("createArchive");
						return -1;
					}
				
					pathindex++;
				}
			}
		
			else if(flags[1])
			{
				while(pathindex<argc)
                                {
					if(listArchive(file, argv[pathindex], flags[3], flags[5])<0)
                                        {       
						close(file);
                                                perror("createArchive");
                                                return -1;
                                        }

                                        pathindex++;
                                }
			}

			else if(flags[2])
			{
				while(pathindex<argc)
                                {	
                			if(extractArchive(file, argv[pathindex], flags[3], flags[5])<0)
                                        {       
						close(file);
                                                perror("createArchive");
                                                return -1;
                                        }

                                        pathindex++;
                                }

			}
		}

                else
                {
                        printUsage();
           		return -1;
                }
	}
	else 
		printUsage();
	
	return 0;
	
}

void printUsage()
{
	fprintf(stderr, "usage: mytar [ctxvS][f tarfile] [ path [ ... ] ]\n");
}

block *makeBlock()
{
	int i;

	block *newBlock=NULL;

	if((newBlock=(block *)malloc(sizeof(block)))==NULL)
		return NULL;
	
	for(i=0; i<512; i++)
		newBlock->data[i]=0;

	newBlock->name = newBlock->data;
	newBlock->mode = newBlock->data + 100;
	newBlock->uid = newBlock->data + 108;
        newBlock->gid = newBlock->data + 116;
        newBlock->size = newBlock->data + 124;
        newBlock->mtime = newBlock->data + 136;
        newBlock->chksum = newBlock->data + 148;
        newBlock->typeflag = newBlock->data + 156;
        newBlock->linkname = newBlock->data + 157;
        newBlock->magic = newBlock->data + 257;
        newBlock->version = newBlock->data + 263;
        newBlock->uname = newBlock->data + 265;
        newBlock->gname = newBlock->data + 297;
        newBlock->devmajor = newBlock->data + 329;
        newBlock->devminor = newBlock->data + 337;
	newBlock->prefix = newBlock->data + 345;

	return newBlock;
}

block *resetBlock(block *old)
{
	int i;

	if(!old)
		return NULL;
	
	for(i=0; i<512; i++)
		old->data[i]=0;

	return old;
}
	
}
