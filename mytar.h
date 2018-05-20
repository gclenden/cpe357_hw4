#ifndef MYTAR_H
#define MYTAR_H

#include <sys/types.h>
#include <dirent.h>
#include <utime.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct blockStruct block;
typedef struct metaDataStruct metaData;

void printUsage();

block *makeBlock();
block *resetBlock(block *old);
metaData *makeMetaData();
metaData *updateMetaData(metaData *myMD, block *header);
int makePath(char *path);
int checkHeader(block *myBlock, int strict);
int argsort(const void *str1, const void *str2);

struct blockStruct
{
	uint8_t data[512];

	uint8_t *name;
	uint8_t *mode;
	uint8_t *uid;
        uint8_t *gid;
        uint8_t *size;
        uint8_t *mtime;
        uint8_t *chksum;
        uint8_t *typeflag;
        uint8_t *linkname;
        uint8_t *magic;
        uint8_t *version;
        uint8_t *uname;
        uint8_t *gname;
        uint8_t *devmajor;
        uint8_t *devminor;
	uint8_t *prefix;

};

struct metaDataStruct
{
	char name[257];
        mode_t mode;
        long int uid;
        long int gid;
        long int size;
       	time_t mtime;
        uint8_t *chksum;
        uint8_t *typeflag;
        char linkname[101];
	char fulllinkname[257];
        uint8_t *magic;
        uint8_t *version;
        uint8_t *uname;
        uint8_t *gname;
        uint8_t *devmajor;
        uint8_t *devminor;
        uint8_t *prefix;
};

#endif
