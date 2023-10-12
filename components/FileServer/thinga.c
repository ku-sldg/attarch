/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>

#define PATH_MAX 4096

int go(char* name)
{
    char* test_name = calloc(1, PATH_MAX);
    /* char* name = "FileSystem/test.test"; */
    int name_len = strnlen(name, PATH_MAX);
    for(int i=0; i<name_len; i++)
    {
        test_name[i] = name[i];
    }
    const char* test_name_const = test_name;

    int size1 = 0;
    bool query_result = file_read_query(test_name_const, &size1);
    if(!query_result)
    {
        printf("Error! No file at that filepath.\n");
        return 0;
    }

    char* file = malloc(size1);
    if(file == NULL)
    {
        printf("Error! Malloc returned NULL.\n");
        return 0;
    }

    for(int i=0; i<size1; i++)
    {
        file[i] = 0x1;
    }

    bool result = file_read_request(test_name_const, &file, size1);
    if(!result)
    {
        printf("Error! No file at that filepath.");
        return 0;
    }

    printf("your file is as follows:\n%s\n", file);

    return 0;
}

int run(void)
{
    go("FileSystem/test.json");
    go("FileSystem/test.test");
    go("FileSystem/test.txt");
    go("FileSystem/new_file_test.value");
}
