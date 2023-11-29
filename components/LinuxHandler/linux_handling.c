/*
 * A simple File Serving component for CAmkES
 * Michael Neises
 * 11 October 2023
 */

#include <camkes.h>

void linux_comm__init(void)
{
    printf("DEBUG: linux_comm_utils interface ready!\n");
}

bool linux_comm_fire_and_forget(const char* message)
{
    printf("fire_and_forget'ing %s\n", message);
    return true;
}

bool linux_comm_receive_request(char** response)
{
    printf("receiving response\n");
    *response = malloc(16);
    (*response)[0] = 'A';
    (*response)[1] = '\0';
    return true;

    /* char* linux_comm_utils_result = read_file_as_string(file_path); */
    /* if(linux_comm_utils_result == NULL) */
    /* { */
    /*     return false; */
    /* } */
    /* for(int i=0; i<size; i++) */
    /* { */
    /*     (*file_contents)[i] = linux_comm_utils_result[i]; */
    /* } */
    /* return true; */

}

bool linux_comm_send_request(const char* ip, const char* port, const char* json_request, char** response)
{
    printf("dispatching...\n");
    printf(" to ip: %s\n", ip);
    printf(" to port: %s\n", port);
    printf(" the request: %s\n", json_request);

    *response = malloc(16);
    (*response)[0] = 'A';
    (*response)[1] = '\0';

    return true;
}
