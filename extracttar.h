#ifndef EXTRACTTAR_H
#define EXTRACTTAR_H

#include <stdio.h>
#include <sys/stat.h>
#include "mytar.h"

int extractArchive(int file, char **argv, int argc, int pathindex, int verbose, int strict);

#endif
