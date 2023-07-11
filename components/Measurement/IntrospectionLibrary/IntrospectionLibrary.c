/*
 * An introspection library for Linux 5.10.y
 * Michael Neises
 * 09 June 2023
 */

#define INTRO_KIMAGE_VADDR 0xffff800010000000

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#include "../hash.h"
#include "../KnownDigests.h"
#include "../configurations/include.h"
#include "AddressTranslation.c"
#include "common/common.c"
#include "measurements.c"

/* This function is here to easily control which measurements */
/* are performed for which versions of linux. */
bool MeasureAndAppraiseLinux()
{
        bool rodata_appraisal = IsKernelRodataOkay(memdev);
        bool modules_appraisal = IsModulesOkay(memdev);
        /* bool tasks_appraisal =  IsTasksOkay(memdev); */

        bool overall_appraisal = true;
        overall_appraisal &= rodata_appraisal;
        overall_appraisal &= modules_appraisal;
        /* overall_appraisal &= tasks_appraisal; */
       
        return overall_appraisal;
}

