#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include "createtar.h"
#include "mytar.h"
#define VERBOSE 0

int createArchive(int file, char *path, int verbose, int strict)
{
        printf("createArchive -- path:%s -- verbose:%i -- strict: %i\n", path, verbose, strict);
	block *header = makeBlock();
	int fd = open(path, O_RDONLY);

	struct stat fileStats;
	if (stat(path, &fileStats) < 0) {
		perror("error in stat buffering\n");
		exit(-1);
	}
	
	/*check size of file path name, and store name and prefix (if necessary)*/
	int pathLen = strlen(path);
	if (pathLen <= 100) {
		strcpy((char*)header->name, path);
	} else {
		strncpy((char*)header->name, path, 100);
		char *prefix = &(path[100]);
		strcpy((char*)header->prefix, prefix);
	}

	printf("%s\n", (char*) header->name);

	/*write octal permissions into a buffer and then write to block*/
	char modeBuff[8];
	int mode = fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	snprintf(modeBuff, 8, "%o", mode);
	strcpy((char*)header->mode, modeBuff);

	/*write octal uid into buffer then write to block*/
	char uidBuff[8];
	int uid = fileStats.st_uid;
	snprintf(uidBuff, 8, "%o", uid);
	strcpy((char*)header->uid, uidBuff);

	/*write octal gid """"" */
	char gidBuff[8];
	int gid = fileStats.st_gid;
	snprintf(gidBuff, 8, "%o", gid);
	strcpy((char*)header->gid, gidBuff);

	/*to conform with return value of snprintf above, size is bytes written to the buff and file_size is the number of bytes in the file. write octal file size to block*/

	char fileSizeBuff[12];
	int fileSize = fileStats.st_size;
	snprintf(fileSizeBuff, 12, "%o", fileSize);
	strcpy((char*)header->size, fileSizeBuff);
	
	/*write octal mtime to block*/
	char mtimeBuff[12];
	int mtime = fileStats.st_mtime;
	snprintf(mtimeBuff, 12, "%o", mtime);
	strcpy((char*)header->mtime, mtimeBuff);

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

	/*writes devmajor to a buff then to the block*/
	char devMajorBuff[8];
	int devMajor = major(fileStats.st_dev);
	snprintf(devMajorBuff, 8, "%o", devMajor);
	strcpy((char*)header->devmajor, devMajorBuff);

	/*writes devminor to a buff then to the block*/
	char devMinorBuff[8];
	int devMinor = minor(fileStats.st_dev);
	snprintf(devMinorBuff, 8, "%o", devMinor);
	strcpy((char*)header->devminor, devMinorBuff);

	/*adds up every uint8_t in the block and places the result in chksum*/
	int i;
	int sum;
	for (i = 0; i < 512; i++) {
		sum += header->data[i];
	}
	char chksumBuff[8];
	snprintf(chksumBuff, 8, "%o", sum);
	strcpy((char*)header->chksum, chksumBuff);

	printf("name: %s\n", header->name);
	printf("mode: %s\n", header->mode);
	printf("uid: %s\n", header->uid);
	printf("gid: %s\n", header->gid);
	printf("mtime: %s\n", header->mtime);
        printf("chksum: %s\n", header->chksum);
        printf("typeflag: %c\n",*(header->typeflag));
        printf("linkname: %s\n", header->linkname);
        printf("magic: %s\n", header->magic);
	printf("verision: %c%c\n", header->version[0], header->version[1]);
        printf("uname: %s\n", header->uname);
        printf("gname: %s\n", header->gname);
	printf("devmajor:  %s\n", header->devmajor);
	printf("devminor:  %s\n", header->devminor);
        printf("prefix: %s\n", header->prefix);

	/*write the header to new file*/
	write(file, &(header->data), 512);

	/*write contents of old file to new file*/
	uint8_t byte;
	block *bodyBuff = resetBlock(header);	
	int index = 0;
	while (read(fd, &byte, 1) != 0) {
		bodyBuff->data[index++] = byte;
		if (index == 512) {
			write(file, &(bodyBuff->data), 512);
			index = 0;
			bodyBuff = resetBlock(bodyBuff);
		}
	}
	write(file, &(bodyBuff->data), 512);


        return 0;
}
