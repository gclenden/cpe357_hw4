#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "createtar.h"
#include "mytar.h"

int createArchive(int file, char *path, int verbose, int strict)
{
        printf("createArchive -- path:%s -- verbose:%i -- strict: %i\n", path, verbose, strict);
	block *header = makeBlock();
	
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

	
        return 0;
}

uint8_t getOctalPermissions()

