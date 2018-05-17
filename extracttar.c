#include <string.h>
#include "extracttar.h"

int extractArchive(int file, char *path, int verbose, int strict)
{
	int nextheader = 0;
	block *newBlock = NULL;
	uint8_t tempsize[13];
	int size;
	int i;		
	int newFile=-1;
	int eoa = 0;
	int pathlen=0;
	int prefixlen=0;
	uint8_t fullName[256];	

	printf("extractArchive\n");
	if((newBlock=makeBlock())==NULL)
		return -1;

	pathlen=strlen(path);
	
	while(read(file, newBlock->data, 512)>0)
	{
		printf("extracting while loop\n");

		for(i=0; i<256; i++)
                	fullName[i]=0;

		if(strcat((char *)fullName, (char *)newBlock->prefix)==NULL || strcat((char *)fullName, (char *)newBlock->name)==NULL)
		{
			free(newBlock);
			return -1;
		}

		printf("fullName: %s\n", fullName);

		if(nextheader == 0)
		{
			if(newFile)
			{
				close(newFile);
				newFile=-1;			
			}

			for(i=0; i<512; i++)
			{
				if(newBlock->data[i])
				{
					eoa=0;
					break;
				}			
			}	

			if(i<512 && ++eoa==2)
			{
				return 0;
			}
			
			printf("header found -- Path: %s -- Prefix: %s -- Name: %s\n", path, (char *)newBlock->prefix, (char *)newBlock->name);

			prefixlen=strlen((char *)newBlock->prefix);
/*
			if(prefixlen == 0 || strncmp((char *)path, (char *)newBlock->prefix, prefixlen)==0)
			{
				if(strncmp((char *)path+prefixlen, (char *)newBlock->name, pathlen-prefixlen)!=0)
				{
					if(verbose)
						printf("%s%s\n", newBlock->prefix, newBlock->name);
					continue;
				}
			}
*/

			if(strncmp((char *)path, (char *)fullName, pathlen)!=0)
				continue;
		
	
			
			printf("\tthe path matches: %s\n", fullName);		

			switch(*(newBlock->typeflag))
			{
				case '0':
				case '\0':
					/*the header is for a file*/
					printf("\tthis is a file\n");

					if((newFile=open((char *)fullName, O_RDONLY | O_TRUNC | O_CREAT, 0666))<0)
		                       	{
						free(newBlock);
                                		return -1;
                        		}

					break;
				case '2':
					printf("\tthis is a symlink\n");
					break;
					/*the header is a for a symbolic link*/
				case '5':
					/*the header if for a directory*/
					printf("\tthis is a directory\n");
					if(mkdir((char *)fullName, 0666)<0)	
					{
						free(newBlock);
						return -1;
					}
				
					break;
				default:
					printf("wrong typeflag format\n");
					return -1;

					
			}

			printf("\tpassed the switch\n");

			tempsize[12]=0;

			if(strncpy((char *)tempsize, (char *)newBlock->size, 12)==NULL)
			{
				free(newBlock);
				return -1;
			}
			
			size=0;
			/*convert the octal ascii size of the file to an integer*/
			for(i=0; i<12; i++)
			{
				size+=power(tempsize[11-i]-'0', i);
			}

			nextheader=size/512;

			if(size%512)
				nextheader+=1;      
		} 

		/*this is data from the last header*/
		else
		{
			if(write(newFile, newBlock->data, 512)!=512)
			{
				free(newBlock);
				close(newFile);
				return -1;
			}


		}
		
	}

	return 0;
}		

int power(int num, int power)
{
	int temp=0;

	while(power--)
	{
		if(temp==0)
			temp=1;
		
		else
			temp*=num;
	}

	return temp;
}

	
