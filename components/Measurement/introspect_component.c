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

    // Execution time without introspection: 0.66s
    // Time to Measure Modules: 0.01s
    // Time to Measure Tasks: 0.86s
    // Time to Measure Kernel Rodata: 0.53s
    while (1)
    {
        printf("DEBUG: Inspector Awake.\n");
        /* Get control from Server AM */
        printf("DEBUG: Inspector waiting for Server signal\n");
        measurement_request_wait();
        printf("DEBUG: Inspector recv Server signal. Emitting to Client.\n");
        /* Pass control to Client AM */
        client_done_emit();
        /* Get control from Client AM */
        printf("DEBUG: Inspector waiting for Client signal\n");
        client_ready_wait();
        printf("DEBUG: Inspector recv Client signal. Emitting to Server.\n");
        /* Pass control to Server AM */
        measurement_done_emit();





        printf("DEBUG: Inspector Start.\n");
        /* ready_wait(); */
        measurement_request_wait();

        bool overall_appraisal = MeasureAndAppraiseLinux();

        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", overall_appraisal ? "Passed" : "Failed.");
        char* resultMsg = overall_appraisal ? "1" : "0";
        // TODO hook the am back up
        /* memset(ms_dp, '0', 4096); */
        /* strcpy(ms_dp, resultMsg); */
        /* done_emit(); */
        measurement_done_emit();
    }
    return 0;
}

