/*
 * A simple File Serving component for CAmkES
 * Michael Neises
 * 11 October 2023
 */

#include <camkes.h>
#include "FileSystem.h"

#define PATH_MAX 4096

void file_read__init(void)
{
    printf("DEBUG: file_read interface ready!\n");
}

/* INPUT: file_path: a string in file-path format, with prefix "FileSystem/"
** ex: "FileSystem/test.json"
** This string should match the folder structure you've setup in your
** "FileSystem" directory.
**
** INPUT/OUTPUT: file_contents: a range of memory with number of bytes equal to
** the "size" input parameter. This range of memory CANNOT be all null bytes.
** For this reason, in the RPC caller, we load this entire range with 0x1
** bytes. My guess is that the seL4 truncates whatever you're "not actually
** using."
**
**
** INPUT: size
** This is the size in bytes of the memory range referred to by the input
** parameter "file_contents." This value should be retrieved via a call to
** "file_read_query." This helps ensure the caller has allocated the correct amount
** of mmeory for this "file_read" procedure. The caller must allocate the
** memory because we cannot clean it up here after we give it away, lest the
** caller lose access prematurely.
**
** RETURN: bool: "if the file_read was successful"
** The only cause for failure should be if the input string is not matched in
** the file system.
** 
*/
bool file_read_request(const char* file_path, char** file_contents, int size)
{
    char* file_read_result = read_file_as_string(file_path);
    if(file_read_result == NULL)
    {
        return false;
    }
    for(int i=0; i<size; i++)
    {
        (*file_contents)[i] = file_read_result[i];
    }
    return true;
}

/* INPUT: file_path: a string in file-path format, with prefix "FileSystem/"
** ex: "FileSystem/test.json"
** This string should match the folder structure you've setup in your
** "FileSystem" directory.
**
** OUTPUT: size: This is the sizeof the statically declared char array referred
** to by the name "file_path"
**
** RETURN: bool: "if the file_query was successful"
** The only cause for failure should be if the input string is not matched in
** the file system.
*/
bool file_read_query(const char* file_path, int* size)
{
    int result = query_size(file_path);
    if(-1 < result)
    {
        (*size) = result;
        return true;
    }
    (*size) = 0;
    return false;
}

