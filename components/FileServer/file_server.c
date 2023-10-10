/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>
#include "FileSystem.h"

#define PATH_MAX 4096

void file_read__init(void)
{
    printf("DEBUG: I'm about to give you your file\n");
}

char* file_read_request(const char* file_path)
{
    printf("DEBUG: here's your file path: %s\n", file_path);
    //char* ret = strdup("this is a test");
    return read_file_as_string(file_path);
}

int run(void)
{
    printf("file server awake\n");
    return 0;
}

