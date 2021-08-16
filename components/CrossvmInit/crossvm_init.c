/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include <camkes.h>

int run(void)
{
    memset(am_dp, '\0', 4096);
    strcpy(am_dp, "This is a crossvm dataport test string\n");

    while (1) {
        ready_wait();
        printf("Got an event\n");
        ((char *)am_dp)[4095] = '\0';
        printf("Response:\n%s\n", (char *)am_dp);
        done_emit();
    }

    return 0;
}
