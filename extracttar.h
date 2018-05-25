#ifndef EXTRACTTAR_H
#define EXTRACTTAR_H

#include <stdio.h>
#include <sys/stat.h>
#include "mytar.h"

int extractArchive(int file, char **argv, int argc, int pathindex, int verbose, int strict);
int handleHeaderEx(int file, int *newFile, char **argv, int argc, int pathindex, int verbose, int strict, int *nextheader, block *newBlock, metaData *newMetaData);
int printType(uint8_t ch);
int printPermissions(char *mode);
int checkMatch(int pathindex, char **argv, int argc, metaData *newMetaData, int pathlen);

#endif
