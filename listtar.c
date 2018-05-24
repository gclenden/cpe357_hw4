#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

#include "listtar.h"

int listArchive(int file, char **argv, int argc, int pathindex, int verbose, int strict)
{	
	int nextheader = 0;
	block *newBlock = NULL;
	metaData *newMetaData = NULL;
	uint8_t buffer[33];
	int i;	
	int eoa = 0;
	int pathlen=0;
	int match=0;
	int result;

	if((newBlock=makeBlock())==NULL)
		return -1;

	if((newMetaData=makeMetaData(newBlock))==NULL)
	{
		free(newBlock);
		return -1;
	}

	while(read(file, newBlock->data, 512)>0)
	{
		
		if(nextheader == 0)
		{
			
			if((result=handleHeader(file, newMetaData, newBlock, argv, pathindex, argc, verbose, strict, &nextheader))<0)
				return -1;
			else if(result==1)
				return 0;
		}

		else 
			nextheader--;

	}
	free(newMetaData);
	free(newBlock);
	return 0;
}

int handleHeader(int file, metaData *newMetaData, block *newBlock, char ** argv, int pathindex, int argc, int verbose, int strict, int *nextheader)
{
	uint8_t buffer[33];
	int i;
	int eoa = 0;
	int pathlen=0;
	int match=0;


	for(i=0; i<512; i++)
	{
		if(newBlock->data[i])
		{
			eoa=0;
			break;
		}
	}

	if(i==512)
	{
		eoa++;
		if(eoa==2)
		{
			free(newMetaData);
			free(newBlock);
			return 1;
		}

		else
			return 0;
	}

	if(checkHeader(newBlock, strict)<0)
	{
		free(newBlock);
		free(newMetaData);
		fprintf(stderr, "invalid header: terminating listing\n");
		return -1;
	}

	if(updateMetaData(newMetaData, newBlock)==NULL)
	{
		free(newMetaData);
		free(newBlock);
		return -1;
	}

	*nextheader=newMetaData->size/512;

	if(newMetaData->size%512)
		*nextheader+=1;

	if(argv!=NULL)
		match=checkMatch(pathindex, argv, argc, newMetaData, pathlen);
	else
		match=1;

	if(!match)
		return 0;

	if(!verbose)
	{
		printf("%s\n", newMetaData->name);
		return 0;
	}

	/*the paths match, extract this entry*/
	if(printType(*(newBlock->typeflag))<0)
		return -1;

	if(printPermissions((char *)newBlock->mode)<0)
		return -1;

	/*print size of file, right aligned in size of 8*/
	char names[66];
	memset(names, 0, sizeof(names));

	if(strcpy(names, (char *)newBlock->uname)==NULL||strcat(names, "/")==NULL||
			strcat(names, (char *)newBlock->gname)==NULL)
	{
		free(newMetaData);
		free(newBlock);
		return -1;
	}

	printf(" %-17s ", names);

	printf("%8ld", newMetaData->size);

	struct tm *info=NULL;


	/*print formatted time of last modification*/

	char buff[267];
	memset(buff, 0, sizeof(buff));

	time_t mod = (time_t)newMetaData->mtime;

	info=localtime(&mod);

	strftime(buff, sizeof(buff), " %Y-%m-%d %H:%M", info);
	printf("%15s ", buff);

	/*print file name*/
	printf("%s\n", newMetaData->name);

	return 0;
}
int checkMatch(int pathindex, char **argv, int argc, metaData *newMetaData, int pathlen)
{
	int i;
	for(i=pathindex; i<argc; i++)
	{
		pathlen=strlen(argv[i]);
		if(pathlen<=256 && strncmp((char *)argv[i], (char *)newMetaData->name, pathlen)==0)
			return 1;        
	}
	return 0;
}

int printType(uint8_t ch)
{
	switch(ch)
	{
		case '0':
		case '\0':
			/*the header is for a file*/
			printf("-");
			break;
		case '2':
			/*the header is for a symlin*/
			printf("l");

			break;
			/*the header is a for a symbolic link*/
		case '5':
			printf("d");
			break;
			/*the header if for a directory*/

		default:
			return -1;                                      
	}

	return 0;
}

int printPermissions(char * mode)
{
	uint8_t buffer[33];
	int i;
	memset(buffer, 0, sizeof(buffer));

	if(strcpy((char *)buffer, mode)==NULL)
		return -1;


	for(i=4;i<7;i++)
	{
		switch(buffer[i])
		{
			case '7':
				printf("rwx");
				break;
			case '6':
				printf("rw-");
				break;
			case '5':
				printf("r-x");
				break;
			case '4':
				printf("r--");
				break;
			case '3':
				printf("-wx");
				break;
			case '2':
				printf("-w-");
				break;   
			case '1':
				printf("--x");
				break;
			default:
				printf("---");

		}       
	}

	return 0;
}
