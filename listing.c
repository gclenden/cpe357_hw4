#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "listing.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void list(char *fileName, int verbose) {

        if (!VERBOSE) {

                /*print file name*/
                printf("%s\n", fileName);

        } else {

                struct stat fileStats;
                if (-1 == stat(fileName, &fileStats)) {
                        printf("stat error\n");
                }

                /*print permissions of the file*/
                printf( (S_ISDIR(fileStats.st_mode)) ? "d" : "-");
                printf( (fileStats.st_mode & S_IRUSR) ? "r" : "-");
                printf( (fileStats.st_mode & S_IWUSR) ? "w" : "-");
                printf( (fileStats.st_mode & S_IXUSR) ? "x" : "-");
                printf( (fileStats.st_mode & S_IRGRP) ? "r" : "-");
                printf( (fileStats.st_mode & S_IWGRP) ? "w" : "-");
                printf( (fileStats.st_mode & S_IXGRP) ? "x" : "-");
                printf( (fileStats.st_mode & S_IROTH) ? "r" : "-");
                printf( (fileStats.st_mode & S_IWOTH) ? "w" : "-");
                printf( (fileStats.st_mode & S_IXOTH) ? "x " : "- ");

                /*get userID, get groupID, strcat them together with a / in between*/
                struct passwd *userInfo = getpwuid(fileStats.st_uid);
                struct group *groupInfo = getgrgid(fileStats.st_gid);
                int userNameLen = strlen(userInfo->pw_name);
                int groupNameLen = strlen(groupInfo->gr_name);
                char *names = (char*) malloc(userNameLen + groupNameLen + 2);
                names = userInfo->pw_name;
                names[userNameLen] = '/';
                strcat(names, groupInfo->gr_name);

                /*print userID/groupID, left aligned in size of 17*/
                printf("%-17s ", names);

                /*print size of file, right aligned in size of 8*/
                printf("%8ld ", fileStats.st_size);

                /*print year of last modification*/
                char buff[16];
                time_t modified = fileStats.st_mtime;
                strftime(buff, 20, "%Y-%m-%d %H:%M ", localtime(&modified));
                printf("%16s ", buff);

                /*print file name*/
                printf("%s\n", fileName);

        }
}
