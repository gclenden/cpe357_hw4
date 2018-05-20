#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "extracttar.h"

int extractArchive(int file, char **argv, int argc, int pathindex, int verbose, int strict)
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
	int match;
	
	if((newBlock=makeBlock())==NULL)
		return -1;

	if((newMetaData=makeMetaData(newBlock))==NULL)
	{
		free(newBlock);
		return -1;
	}
	
	while(read(file, newBlock->data, 512)>0)
	{
		memset(fullLink, 0, sizeof(fullLink));

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

			if(checkHeader(newBlock, strict)<0)
			{
				free(newMetaData);
				free(newBlock);
				fprintf(stderr, "invalid header, ending extract\n");
				return 0;
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


			match=0;
                        if(argv!=NULL)
                        {
                                for(i=pathindex; i<argc; i++)
                                {
                                        pathlen=strlen(argv[i]);
                                        if(pathlen<=256 && strncmp((char *)argv[i], (char *)newMetaData->name, pathlen)==0)
                                                match=1;
                                }
                        }

                        else
                                match=1;

                        if(!match)
                                continue;

/*			if(pathlen>255 || strncmp((char *)path, (char *)newMetaData->name, pathlen)!=0)
				continue;	
*/			
			if(verbose)
				printf("%s\n", newMetaData->name);

			makePath(newMetaData->name);
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
					/*this is a symbolic link*/
					makePath((char *)newMetaData->fulllinkname);

					if(symlink(newMetaData->linkname, newMetaData->name)<0 && (errno!=EEXIST))
					{
						printf("failed to make symlink\n");
						free(newMetaData);
						free(newBlock);
						return -1;
					}		

					break;
					
				case '5':
					break;
					/*the header if for a directory*/
								
				default:
					return -1;					
			}
			
			/*update the mod time*/
			if(*(newBlock->typeflag)!='2')
			{
                        	modtime.actime=newMetaData->mtime;
                        	modtime.modtime=newMetaData->mtime;
                        	if(utime(newMetaData->name, &modtime)<0)
                        	{
	                        	free(newBlock);
        	                	free(newMetaData);
                                	return -1;
                		}	

			}
		} 

		/*this is data the goes in the file from the last header*/
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
