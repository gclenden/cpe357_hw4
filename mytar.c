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
			{
				dup2(i, STDOUT_FILENO);
				close(i);
			}

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
	printf("usage: mytar [ctxvS][f tarfile] [ path [ ... ] ]\n");
}

void listVerbose(char *fileName) {
	struct stat fileStat;
	if (-1 == stat(fileName, &fileStat)) {
		printf("stat error\n");
	}

	/*print permissions of the file*/
	printf( (S_ISDIR(fileStats.st_mode)) ? "d" : "-");
    	printf( (fileStats.st_mode & S_IRUSR) ? "r" : "-");
    	printf( (fileStats.st_mode & S_IWUSR) ? "w" : "-");
    	printf( (fileStats.st_mode & S_IXUSR) ? "x" : "-");
    	printf( (fileStats.st_mode & S_IRGRP) ? "r" : "-");
    	printf( (fileStats.st_mode & S_IWGRP) ? "w" : "-");
    	printf( (fileStats.st_mode & S_IXGRP) ? "x" : "-");
    	printf( (fileStats.st_mode & S_IROTH) ? "r" : "-");
    	printf( (fileStats.st_mode & S_IWOTH) ? "w" : "-");
    	printf( (fileStats.st_mode & S_IXOTH) ? "x" : "-");

	/*get userID, get groupID, strcat them together with a / in between*/
	struct passwd *userInfo = getpwuid(fileStats.st_uid);
	struct group *groupInfo = getgrgid(fileStats.st_gid);
	int userNameLen = strlen(userInfo->pw_name);
	int groupNameLen = strlen(groupInfo->gr_name);
	char *names = (char*) malloc(userNameLen + groupNameLen + 2);
	names = userInfo->pw_name;
	names[userNameLen] = '/';
	strcat(names, groupInfo->gr_name);

	/*print userID/groupID, left aligned in size of 17*/
	printf("%-17s ", names);

	/*print size of file, right aligned in size of 8*/
	printf("%8ld ", fileStat.st_size);

	/*print year of last modification*/
	char buff[20];
	time_t modified = fileStat.st_mtime;
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S ", localtime(&now));
	
	/*print file name*/
	printf("%s\n", fileName);
	
int createArchive(int file, char *path, int verbose, int strict)
{
	printf("path:%s\n", path);
	return 0;
}

int listArchive(int file, char *path, int verbose, int strict)
{
	printf("path:%s\n", path);
	return 0;
}

int extractArchive(int file, char *path, int verbose, int strict)
{
	printf("path:%s\n", path);
	return 0;
}
