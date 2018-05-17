#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "createtar.h"
#include "mytar.h"
#define VERBOSE 0

/*main function to test createArchive*/
int main(int argc, char *argv[]){
	
	int fd = fileno(".");	
	int archive = createArchive(fd, "//home/cadaly/CSC357/hw4", 0, 0);

	return 0;

}

int createArchive(int file, char *path, int verbose, int strict)
{
        printf("createArchive -- path:%s -- verbose:%i -- strict: %i\n", path, verbose, strict);
	block *header = makeBlock();i
	
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
		strncpy((char*)header->name, path, 100)
		char *prefix = path[100];
		strcpy((char*)header->prefix, prefix);
	}

	printf("%s\n", (char*) header->name);

	/*write octal permissions into a buffer and then write to block*/
	char modeBuff[8];
	int mode = fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	int size = snprintf(modeBuff, 8, "%o", mode);
	strcpy((char*)header->mode, modeBuff);

	/*write octal uid into buffer then write to block*/
	char uidBuff[8];
	int uid = fileStats.st_uid;
	size = snprintf(uidBuff, 8, "%o", uid)
	strcpy((char*)header->uid, uidBuff);

	/*write octal gid """"" */
	char gidBuff[8];
	int gid = fileStats.st_gid;
	size = snprintf(gidBuff, 8, "%o", gid);
	strcpy((char*)header->gid, gidBuff);

	/*to conform with return value of snprintf above, size is bytes written to the buff and file_size is the number of bytes in the file. write octal file size to block*/

	char fileSizeBuff[12];
	int fileSize = fileStats.st_size;
	size = snprintf(fileSizeBuff, 12, "%o", fileSize);
	strcpy((char*)header->size, fileSizeBuff);
	
	/*write octal mtime to block*/
	char mtimeBuff[12]
	int mtime = fileStats.st_mtime;
	size = snprintf(mtimeBuff, 12, "%o", mtime);
	strcpy((char*)header->mtime, mtimeBuff);

	/*see chksum filled in at the end, depends on all other fields being completed*/
	

	/*check for file type and write corresponding ascii number to block*/

	char type;
	if (S_ISLNK(file_stats.st_mode)) {
		type = '2';
	} else if (S_ISREG(file_stats.st_mode)) {	
		type = '0';
	} else if(S_ISDIR(file_stats.st_mode)) {
		type = '5';
	}
	header->typeflag = type;

	/*if file is a symbolic link, get its name into buffer and put it in block*/
	if (type == '2') {
		char link[100];
		int linkSize = readlink(path, link, 100);
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
	strcpy(header->uname, unameBuff);

	/*get group name into gnameBuff and then write to block*/
	char gnameBuff[32]; 
        struct group *groupInfo = getgrgid(fileStats.st_gid);
        strncpy(gnameBuff, groupInfo->gr_name, 31);
	gnameBuff[32] = '\0';
	strcpy(header->gname, gnameBuff);

	/*wasn't sure what to do for devmajor and devminor yet... think they might just be all 0's everytime since I think they only have to do with block special files, which are unsupported in hw4, but not 100% sure*/

        return 0;
}

