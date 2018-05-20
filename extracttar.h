#ifndef EXTRACTTAR_H
#define EXTRACTTAR_H

#include <stdio.h>
#include <sys/stat.h>
#include "mytar.h"

int extractArchive(int file, char *path, int verbose, int strict);
int power(int, int);
#endif
