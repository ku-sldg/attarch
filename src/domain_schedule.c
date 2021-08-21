/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

/* This is a domain schedule that is suitable for the domains tests in sel4test. All
 * sel4test actually needs is for every domain to be executable for some period of time
 * in order for the tests to make progress
 */

/* remember that this is compiled as part of the kernel, and so is referencing kernel headers */

#include <config.h>
#include <object/structures.h>
#include <model/statedata.h>

const dschedule_t ksDomSchedule[] = {
    // VM domain
    { .domain = 0, .length = 5 },
    // PlatAM domain
    { .domain = 1, .length = 1 },
};

const word_t ksDomScheduleLength = sizeof(ksDomSchedule) / sizeof(dschedule_t);