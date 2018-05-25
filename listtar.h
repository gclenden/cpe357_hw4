#ifndef LISTTAR_H
#define LISTTAR_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "listtar.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include "mytar.h"

int listArchive(int file, char **argv, int argc, int pathindex, int verbose, int strict);
int handleHeader(int file, metaData *newMetaData, block *newBlock, char ** argv, int pathindex, int argc, int verbose, int strict, int *nextheader);
int checkMatch(int pathindex, char **argv, int argc, metaData *newMetaData, int pathlen);
int printType(uint8_t ch);
int printPermissions(char * mode);


#endif
