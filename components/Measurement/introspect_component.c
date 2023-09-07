/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>
#include "configurations/include.h"
#include "hash.h"
#include "IntrospectionLibrary/IntrospectionLibrary.c"

int run(void)
{
    ShaTest();

    printf("DEBUG: Inspector Awake. Waiting for Server signal\n");
    measurement_request_wait();

    char* msg_test = calloc(1,4096);
    strcpy(msg_test, server_dp);
    printf("recv from server: %s\n", strcmp(msg_test,"1") == 0 ? "Good" : "Failed");

    msg_test = "3";
    memset(server_dp, '0', 4096);
    strcpy(server_dp, msg_test);
    
    msg_test = "4";
    memset(client_dp, '0', 4096);
    strcpy(client_dp, msg_test);

    printf("DEBUG: Inspector recv Server signal. Emitting to Client.\n");
    client_done_emit();
    printf("DEBUG: Inspector waiting for Client signal\n");
    client_ready_wait();
    printf("DEBUG: Inspector recv Client signal. Emitting to Server.\n");

    strcpy(msg_test, client_dp);
    printf("recv from client: %s\n", msg_test);
    printf("recv from client: %s\n", strcmp(msg_test,"6") == 0 ? "Good" : "Failed");

    measurement_done_emit();


    // Execution time without introspection: 0.66s
    // Time to Measure Modules: 0.01s
    // Time to Measure Tasks: 0.86s
    // Time to Measure Kernel Rodata: 0.53s
    while (1)
    {





        printf("DEBUG: Inspector Start.\n");
        /* ready_wait(); */
        measurement_request_wait();

        bool overall_appraisal = MeasureAndAppraiseLinux();

        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", overall_appraisal ? "Passed" : "Failed.");
        char* resultMsg = overall_appraisal ? "1" : "0";
        // TODO hook the am back up
        memset(server_dp, '0', 4096);
        strcpy(server_dp, resultMsg);
        /* done_emit(); */
        measurement_done_emit();
    }
    return 0;
}

