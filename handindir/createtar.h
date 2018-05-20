#ifndef CREATETAR_H
#define CREATETAR_H

int createArchive(int file, char *path, int verbose, int strict);

int prefix(char *path);

int insert_special_int(char *where, size_t size, int32_t val);

#endif
