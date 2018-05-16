#ifndef MYTAR_H
#define MYTAR_H

typedef struct blockstruct block;

void printUsage();
int createArchive(int file, char *path, int verbose, int strict);
int listArchive(int file, char *path, int verbose, int strict);
int extractArchive(int file, char *path, int verbose, int strict);
block *makeBlock();

struct blockstruct
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

#endif
