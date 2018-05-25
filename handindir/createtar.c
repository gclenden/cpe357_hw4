#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include "createtar.h"
#include "mytar.h"
#define VERBOSE 0

int createArchive(int file, char *path, int verbose, int strict)
{
	
        printf("createArchive -- path:%s -- verbose:%i -- strict: %i\n", path, verbose, strict);
	
	if (strlen(path) > 256) {
		fflush(stdout);
		perror("Path name must be shorter than 256 characters\n");
		return -1;
	}
	
	

	block *header = makeBlock();
	int fd;
	if ((fd = open(path, O_RDONLY)) < 0) {
		fflush(stdout);
		perror("File Nonexistant");
		return 0;
	}
		

	struct stat fileStats;
	if (stat(path, &fileStats) < 0) {
		fflush(stdout);
		perror("error in stat buffering\n");
		exit(-1);
	}
	
	/*check size of file path name, and store name and prefix (if necessary)*/
	fflush(stdout);
	int split = prefix(path);
	fflush(stdout);
	if (split == -1) {
		strcpy((char*)header->name, path);
		if (S_ISDIR(fileStats.st_mode)) {
			header->name[strlen(path)] = '/';
		}
	} else {
		strncpy((char*)header->prefix, path, split);
		char *name = &(path[split + 1]);
		strcpy((char*)header->name, name);
		if (S_ISDIR(fileStats.st_mode)) {
			header->name[strlen(name)] = '/';
		}
	}


	/*write octal permissions into a buffer and then write to block*/
	char modeBuff[8];
	char modeTestBuff[10];
	int mode = fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	snprintf(modeTestBuff, 10, "%o", mode);
	int bufflen = strlen(modeTestBuff);
	if (bufflen > 7) {
		if (insert_special_int((char*)header->mode, 8, mode) != 0)
			printf("error inserting special int\n");	
	} else {
		snprintf(modeBuff, 8, "%07o", mode);
        	strcpy((char*)header->mode, modeBuff);
	}

	/*write octal uid into buffer then write to block*/
	char uidBuff[8];
	char uidTestBuff[10];
	int uid = fileStats.st_uid;
	snprintf(uidTestBuff, 10, "%o", uid);
	int uidlen = strlen(uidTestBuff);
	if (uidlen > 7) {
		if (insert_special_int((char*)header->uid, 8, uid) != 0) 
                        printf("error inserting special int\n");
	} else {
		snprintf(uidBuff, 8, "%07o", uid);
        	strcpy((char*)header->uid, uidBuff);
	}
	
	/*write octal gid """"" */
	char gidBuff[8];
	char gidTestBuff[10];
	int gid = fileStats.st_gid;
	snprintf(gidTestBuff, 10, "%o", gid);
	int gidlen = strlen(gidTestBuff);
	if (gidlen > 7) {
		if (insert_special_int((char*)header->gid, 8, gid) != 0) 
                        printf("error inserting special int\n");
	} else {
		snprintf(gidBuff, 8, "%07o", gid);
        	strcpy((char*)header->gid, gidBuff);
	}

	/*to conform with return value of snprintf above, size is bytes written to the buff and file_size is the number of bytes in the file. write octal file size to block*/

	char fileSizeBuff[12];
	char fileSizeTestBuff[14];
	int fileSize;
	if (S_ISDIR(fileStats.st_mode)) {
		fileSize = 0;
	} else {
		fileSize = fileStats.st_size;
	}
	snprintf(fileSizeTestBuff, 14, "%o", fileSize);
	int filesizelen = strlen(fileSizeTestBuff);
	if (filesizelen > 11) {
		if (insert_special_int((char*)header->size, 12, fileSize) != 0) 
                        printf("error inserting special int\n");
	} else {
		snprintf(fileSizeBuff, 12, "%011o", fileSize);
        	strcpy((char*)header->size, fileSizeBuff);
	}
	
	/*write octal mtime to block*/
	char mtimeBuff[12];
	char mtimeTestBuff[14];
	int mtime = fileStats.st_mtime;
	snprintf(mtimeTestBuff, 14, "%o", mtime);
	int mtimelen = strlen(mtimeTestBuff);
	if (mtimelen > 11) {
		if (insert_special_int((char*)header->mtime, 12, mtime) != 0) 
                        printf("error inserting special int\n");
	} else {
		snprintf(mtimeBuff, 12, "%0o", mtime);
        	strcpy((char*)header->mtime, mtimeBuff);
	}

	/*see chksum filled in at the end, depends on all other fields being completed*/
	

	/*check for file type and write corresponding ascii number to block*/

	char type;
	if (S_ISLNK(fileStats.st_mode)) {
		type = '2';
	} else if (S_ISREG(fileStats.st_mode)) {	
		type = '0';
	} else if(S_ISDIR(fileStats.st_mode)) {
		type = '5';
	}
	*(header->typeflag) = type;

	/*if file is a symbolic link, get its name into buffer and put it in block*/
	if (type == '2') {
		char link[100];
		readlink(path, link, 100);
		strcpy((char*)header->linkname, link);
	
	}

	/*write magic num and version, always "ustar" (null-terminated) and "00" respectively*/
	strcpy((char*)header->magic, "ustar");
	header->version[0] = '0';
	header->version[1] = '0';

	/*get user name into unameBuff and then write to block*/
	char unameBuff[32];
	struct passwd *userInfo = getpwuid(fileStats.st_uid);
	strncpy(unameBuff, userInfo->pw_name, 31);
	unameBuff[32] = '\0';
	strcpy((char*)header->uname, unameBuff);

	/*get group name into gnameBuff and then write to block*/
	char gnameBuff[32]; 
        struct group *groupInfo = getgrgid(fileStats.st_gid);
        strncpy(gnameBuff, groupInfo->gr_name, 31);
	gnameBuff[32] = '\0';
	strcpy((char*)header->gname, gnameBuff);

	/*adds up every uint8_t in the block and places the result in chksum*/
	int i;
	int sum = 0;
	for (i = 0; i < 512; i++) {
		sum += header->data[i];
	}
	char chksumBuff[8];
	sum += (' ' * 8);
	snprintf(chksumBuff, 8, "%07o", sum);
	strcpy((char*)header->chksum, chksumBuff);


	/*write the header to new file*/
	write(file, &(header->data), 512);

	/*write contents of old file to new file*/
	block *bodyBuff = resetBlock(header);	
	while (read(fd, &(bodyBuff->data), 512) > 0) {
		write(file, &(bodyBuff->data), 512);
		bodyBuff = resetBlock(bodyBuff);
	}
	DIR *currDir;	
	if((currDir=opendir(path))==NULL)
	{
		if(errno&ENOTDIR)
			return 0;
		return -1;
	}

	struct dirent *nextDir;
	while((nextDir=readdir(currDir))!=NULL)
	{
		
		if(strcmp(nextDir->d_name, ".")!=0 && strcmp(nextDir->d_name, "..")!=0) {
			char pathBuff[256] = {0};
			strcpy(pathBuff, path);
			pathBuff[strlen(path)] = '/';
			if (strlen(path) + strlen(nextDir->d_name) <= 255) {
				strcat(pathBuff, nextDir->d_name);
			} else {
				perror("path name must be shorter than 256 characters\n");
                		return -1;
			}
			createArchive(file, pathBuff, verbose, strict);
			
		}
	}
	

        return 0;
}

/*returns the last index of prefix path if there should be a prefix, -1 if no prefix*/

int prefix(char *path) {
	int len = strlen(path);
	int lastIndex = -1;
	if (len <= 100) {
		return -1;
	} else {
		int index = 0;
		while (lastIndex == -1) {
			if (path[index] == '/' && (len - index <= 100)) {
				lastIndex = index;
			}
			index++;
		}
	}
	return lastIndex;
}


/*places binary integer version of number if octal string too large*/
int insert_special_int(char *where, size_t size, int32_t val) { 
	int err=0;
	if(val<0||(size<sizeof(val)) ){
		err++;
	} else {
		memset(where, 0, size);
		*(int32_t *)(where+size-sizeof(val)) = htonl(val);
		*where |= 0x80;
	}
	return err; 
}
