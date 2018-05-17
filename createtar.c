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
	if (fstat(file, &fileStats) < 0) {
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
	char mode[8];
	int permissions = fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	int size = snprintf(mode, 8, "%o", permissions);
	strcpy((char*)header->mode, mode);

	
        return 0;
}

