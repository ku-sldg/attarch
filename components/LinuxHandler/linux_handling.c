/*
 * A simple File Serving component for CAmkES
 * Michael Neises
 * 11 October 2023
 */

#include <camkes.h>
#include "dataports.c"

#define MESSAGE_SIZE 4096

void linux_comm__init(void)
{
    InitDataports();
    printf("DEBUG: linux_comm_utils interface ready!\n");
}

bool linux_comm_fire_and_forget(const char* message)
{
    bool result = WriteLinuxDataport(message);
    if(!result)
    {
        printf("ERROR: Failed to Write the Linux Dataport\n");
        return false;
    }
    return true;
}

bool linux_comm_receive_request(char** response)
{
    /* printf("Waiting to receive 4096 bytes of response...\n"); */
    WaitLinuxDataport();
    /* printf("Receiving...\n"); */

    /* We, the callee, allocate this memory. The caller must free it. */
    *response = calloc(1, MESSAGE_SIZE);
    uint8_t* contents = calloc(1, MESSAGE_SIZE);
    bool result = ReadLinuxDataport(contents);
    if(!result)
    {
        printf("ERROR: Failed to Read the Linux Dataport\n");
        return false;
    }
    for(int i=0; i<4096; i++)
    {
        (*response)[i] = contents[i];
    }
    free(contents);

    return true;
}

bool linux_comm_send_request(const char* ip, const char* port, const char* json_request, char** response)
{
    printf("dispatching...\n");
    printf(" to ip: %s\n", ip);
    printf(" to port: %s\n", port);
    printf(" the request: %s\n", json_request);
    printf("Just kidding. It's going to the linux VM.\n");

    bool result = linux_comm_fire_and_forget(json_request);
    if(!result)
    {
        printf("Failed to Send Request to the Linux Dataport\n");
        return false;
    }
    return true;
}
