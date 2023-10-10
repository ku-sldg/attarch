/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>

#define PATH_MAX 4096

int run(void)
{
    printf("thinga awake\n");

    char* test_name = calloc(1, PATH_MAX);
    char* name = "FileSystem/test.json";
    int name_len = strnlen(name, PATH_MAX);
    for(int i=0; i<name_len; i++)
    {
        test_name[i] = name[i];
    }
    const char* test_name_const = test_name;



    char* file = file_read_request(test_name_const);

    printf("your file is as follows: %s\n", file);

    return 0;
}

