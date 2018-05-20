#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "mytar.h"
#include "listtar.h"
#include "extracttar.h"
#include "createtar.h"

int main(int argc, char** argv)
{
	uint8_t flags[6]={0}; /*[c, t, x, v, f, S]*/
	int i;
	int ch;
	int pathindex=2;
	int file=STDOUT_FILENO;
	
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
/*	
		if(flags[4])
		{
			if((file=open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0666))<0)
			{
				perror("open f");
				return -1;
			}

			else

			pathindex++;
		}

		else if(!flags[4])
		{
			pathindex=2;
		}

		else
		{
			printUsage();
			close(STDOUT_FILENO);
			return -1;
		}
*/
		/* see what function I'll need to callf from here*/
                if(flags[0] ^  flags[1] ^ flags[2])
                {
                        /*we know what function to do*/
			/*createArchive*/
			if(flags[0])
			{
				
				if(flags[4])
                		{
					pathindex++;
                        		if((file=open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666))<0)
                        		{
                                		perror("create archive open f");
                                		return -1;
                        		}
				}
				
				/*no path was provided, just return*/
				if(pathindex==argc)
					return 0;         	
	
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
			
				block *empty= makeBlock();
				write(file, &(empty->data), 512);
				write(file, &(empty->data), 512);
			}
		
			/*listArchive*/
			else if(flags[1])
			{
				if(flags[4])
                                {
                                        pathindex++;
                                        if((file=open(argv[2], O_RDONLY, 0666))<0)
                                        {
						printf("couldn't open the file\n");
                                                printUsage();
                                                return -1;
                                        }
                                }

                                /*no path was provided, print everything*/
                                if(pathindex==argc)
                                {
				        if(listArchive(file, "", flags[3], flags[5])<0)
                                        {       
                                                close(file);
                                                perror("listArchive");
                                                return -1;
                                        }
				}

				else
				{	while(pathindex<argc)
                                	{
						lseek(file, 0, SEEK_SET);
						if(listArchive(file, argv[pathindex], flags[3], flags[5])<0)
	                                        {       
							close(file);
	                                                perror("listArchive");
	                                                return -1;
	                                        }
	
	                                        pathindex++;
	                                }
				}
			}

			/*extractArchive*/
			else if(flags[2])
			{
				printf("about to open the extract file\n");
				if(flags[4])
                                {
                                        pathindex++;
                                        if((file=open(argv[2], O_RDONLY, 0666))<0)
                                        {
                                                printUsage();
                                                return -1;
                                        }
                                }

                                /*no path was provided, print everything*/
				if(pathindex==argc)
                                {
					printf("main: no path provided, extract all\n");
                                        if(extractArchive(file, "", flags[3], flags[5])<0 && errno)
                                        {
                                                close(file);
                                                perror("extractArchive");
                                                return -1;
                                        }
                          	}
				
				while(pathindex<argc)
                                {
					lseek(file, 0, SEEK_SET);
					printf("main: looping through all paths to extract\n");	
                			if(extractArchive(file, argv[pathindex], flags[3], flags[5])<0)
                                        {       
						close(file);
                                                perror("extractArchive");
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
	block *newBlock=NULL;

	if((newBlock=(block *)malloc(sizeof(block)))==NULL)
		return NULL;
	
	memset(newBlock, 0, sizeof(block));	

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
	if(!old)
		return NULL;
	
	memset(old, 0, 512);

	return old;
}

metaData *makeMetaData(block *myBlock)
{
	metaData *newMD=NULL;

	if(!myBlock)
		return NULL;
	
	newMD = (metaData *)malloc(sizeof(metaData));
	if(!newMD)
		return NULL;
	
	memset(newMD, 0, sizeof(metaData));

	return newMD;
}

metaData *updateMetaData(metaData *myMD, block *header)
{
	char buffer[33];
	int i=0;	
	int pathlen=0;
		
        memset(myMD, 0, sizeof(metaData));
	
	if(strlen((char *)header->prefix))
	{
        	if(strncpy((char *)myMD->name, (char *)header->prefix, 155)==NULL || 
			strcat((char *)myMD->name, "/")==NULL)
		{
			free(myMD);
			return NULL;
		}

		if(strncat((char *)myMD->name, (char *)header->name, 100)==NULL)
        	{
                	free(myMD);
                	return NULL;
        	}
	}
		
	else if(strncpy(myMD->name, (char *)header->name, 100)==NULL)
	{
		free(myMD);
		return NULL;
	}
		
	if(strcpy(buffer, (char *)header->mode)==NULL)
	{
		free(myMD);
		return NULL;
	}
	
	/*check for an execute permissions*/
	if(((buffer[4]-'0')%2)||((buffer[4]-'0')%2)||((buffer[6]-'0')%2))
		myMD->mode=0777;
	
	else
		myMD->mode=0666;

        if(! strncpy(buffer, (char *)header->size, 12))
        {
        	free(myMD);
                return NULL;
        }

        myMD->size=strtol(buffer, NULL, 8);

	if(strncpy(buffer, (char *)header->mtime, 12)==NULL)
        {
                free(myMD);
                return NULL;
        }

	myMD->mtime=strtoll(buffer, NULL, 8);
	
	if(header->linkname[0]=='/' && strncpy((char *)myMD->fulllinkname, (char *)header->linkname, 100)==NULL)
	{
		free(myMD);
		return NULL;
	}

	else 
	{
		while(myMD->name[i]!='\0' && i<256)
		{
			if(myMD->name[i]=='/')
				pathlen=i;

			i++;
		}

		if(strncpy((char *)myMD->fulllinkname, (char *)myMD->name, pathlen)==NULL
			|| strncat((char *)myMD->fulllinkname, (char *)header->linkname, 100) == NULL)
		{
			free(myMD);
			return NULL;
		}
		
	}	
	return myMD;
}

DIR *makePath(char *path)
{
	DIR *tempDir=NULL;
	int i=0;
	
	while(path[i]!=0)
	{
		if(path[i]=='/')
		{
			if(tempDir)
			{
				closedir(tempDir);
				tempDir=NULL;
			}

			path[i]=0;
			printf("\tmakePath: %s\n", path);
			if(mkdir(path, 0777)<0 &&  !(errno&EEXIST))
				return NULL;

			tempDir=opendir(path);
			
			path[i]='/';
		}
		
		i++;	
        }
	
	return tempDir;
}

