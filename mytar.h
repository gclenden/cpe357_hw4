#ifndef MYTAR_H
#define MYTAR_H

void printUsage();
int createArchive(int file, char *path, int verbose, int strict);
int listArchive(int file, char *path, int verbose, int strict);
int extractArchive(int file, char *path, int verbose, int strict);

#endif
