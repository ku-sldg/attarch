#ifndef DATAPORT_INTERFACE_H
#define DATAPORT_INTERFACE_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

// not called PAGE_SIZE bc something else claimed that name already
#define DP_PAGE_SIZE 4096
typedef uint8_t (*PagePointer)[DP_PAGE_SIZE];
typedef enum {
    SUCCESS = 0,
    NULL_INPUT = 1,
    FILE_NOT_FOUND = 2, 
    FILE_NOT_MAPPED = 3,
    FILE_READ_FAILURE = 4,
} dpResult;
typedef dpResult (*DataportAction)(PagePointer, PagePointer);
typedef dpResult (*FileAction)(int);

void HandleDataportResult(dpResult err);
dpResult writeAction(PagePointer dp, PagePointer msg);
dpResult readAction(PagePointer dp, PagePointer msg);
dpResult waitAction(int fd);
dpResult DoWithDataport(int fd, int flags, DataportAction dpJob, PagePointer msg);
dpResult DoWithFile(const char* name, int file_flags, int dataport_flags, PagePointer msg, FileAction fileJob, DataportAction dpJob);
dpResult write_dataport(const char *name, PagePointer msg);
dpResult read_dataport(const char *name, PagePointer msg);
dpResult emit_dataport(const char *name);
dpResult wait_dataport(const char *name);

#endif
