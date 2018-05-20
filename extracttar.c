#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "extracttar.h"

int extractArchive(int file, char *path, int verbose, int strict)
{	
	struct utimbuf modtime;
	int nextheader = 0;
	block *newBlock = NULL;
	metaData *newMetaData = NULL;
	uint8_t fullLink[257];
	int i;		
	int newFile=-1;
	int eoa = 0;
	int pathlen=0;
	int writesize;
	DIR *newDir=NULL;

	if((newBlock=makeBlock())==NULL)
		return -1;

	if((newMetaData=makeMetaData(newBlock))==NULL)
	{
		free(newBlock);
		return -1;
	}
	
	pathlen=strlen(path);
	
	while(read(file, newBlock->data, 512)>0)
	{
		memset(fullLink, 0, sizeof(fullLink));
/*
		for(i=0; i<256; i++)
                	fullName[i]=0;

		for(i=0; i<32; i++)
			buffer[i]=0;
*/
/*
		if(strcat((char *)newMetaData->name, (char *)newBlock->prefix)==NULL || 
		   strcat((char *)newMetaData->name, (char *)newBlock->name)==NULL)
		{
			free(newBlock);
			return -1;
		}
*/

		if(nextheader == 0)
		{
			if(newFile>0)
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

			if(i==512)
			{
				eoa++;
				if(eoa==2)
				{
					free(newMetaData);
					free(newBlock);
					return 0;
				}

				else
					continue;
			}

			if(updateMetaData(newMetaData, newBlock)==NULL)
			{
				free(newMetaData);
				free(newBlock);
				return -1;
			}			

			nextheader=newMetaData->size/512;

                        if(newMetaData->size%512)
                                nextheader+=1;

                        /*
			prefixlen=strlen((char *)newBlock->prefix);

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
			if(pathlen>255 || strncmp((char *)path, (char *)newMetaData->name, pathlen)!=0)
				continue;	
			
			if(verbose)
				printf("%s\n", newMetaData->name);

			newDir = makePath(newMetaData->name);
			printf("newDir: %p\n", (void *)newDir);
			fflush(stdout);
			/*makePath(path);*/

			/*the paths match, extract this entry*/
			switch(*(newBlock->typeflag))
			{
				case '0':
				case '\0':
					/*the header is for a file*/
					if((newFile=open((char *)newMetaData->name, O_WRONLY | O_TRUNC | 
								O_CREAT, newMetaData->mode))<0)
		                       	{
						free(newBlock);
						free(newMetaData);
                                		return -1;
                        		}

					break;
				case '2':
					printf("making a new symlink -- linkname: %s\n", newBlock->linkname);
					/*makePath((char *)newBlock->linkname);
				
					*/
				
					newFile=dirfd(newDir);
					if(symlinkat(newMetaData->name, newFile, (char *)newMetaData->fulllinkname)<0 && (errno!=EEXIST))
					{
						printf("failed to make symlink");
						free(newMetaData);
						free(newBlock);
						return -1;
					}
	
					newFile=-1;
					
					printf("making a new file at the symlink\n");
                                        /*the header is for a file*/
                                        /*if((newFile=open((char *)newMetaData->name, O_WRONLY | O_TRUNC |
                                                                O_CREAT, newMetaData->mode))<0)
                                        {
                                                free(newBlock);
                                                free(newMetaData);
                                                return -1;
                                        }
					*/
					

					break;
					/*the header is a for a symbolic link*/
				case '5':
					break;
					/*the header if for a directory*/
					/*if(mkdir((char *)newMetaData->name, newMetaData->mode)<0)	
					{
						perror("couldn't mkdir");
						if(!(errno&EEXIST))
						{
							free(newMetaData);
							free(newBlock);
							return -1;
						}
						printf("moving on because it was already made\n");
					}	
				
					break;*/
				default:
					return -1;					
			}
			/*update the mod time*/
                        modtime.actime=newMetaData->mtime;
                        modtime.modtime=newMetaData->mtime;
                        if(utime(newMetaData->name, &modtime)<0)
                        {
	                        free(newBlock);
        	                free(newMetaData);
                                return -1;
                	}

			closedir(newDir);

/*			tempsize[11]=0;
			if(! strncpy((char *)tempsize, (char *)newBlock->size, 11))
			{
				free(newBlock);
				return -1;
			}

			printf("the tempsize: %s\n", (char *)tempsize);

			size=strtol((char *)tempsize, NULL, 8);
			
			
			nextheader=newMetaData->size/512;

			if(newMetaData->size%512)
				nextheader+=1;

			printf("\t header size: %li -- nextheader: %i\n", newMetaData->size, nextheader);      
*/		} 

		/*this is data from the last header*/
		else
		{
			if(nextheader>1)
				writesize=512;
			
			else
				writesize=newMetaData->size%512;
			
			if(newFile>0)
			{
				if(write(newFile, newBlock->data, writesize)!=writesize)
				{
					free(newMetaData);
					free(newBlock);
					close(newFile);
					return -1;
				}

				/*update the mod time*/
                        	modtime.actime=newMetaData->mtime;
                        	modtime.modtime=newMetaData->mtime;
                        	if(utime(newMetaData->name, &modtime)<0)
                        	{
                        	        free(newBlock);
                        	        free(newMetaData);
                        	        return -1;
                        	}
			}

			nextheader--;
		}
	}
	free(newMetaData);
	free(newBlock);
	return 0;
}			
