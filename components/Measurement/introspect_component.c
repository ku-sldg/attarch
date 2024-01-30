/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>
#include <stdbool.h> // for bool type
#include <stdint.h> // for uint64_t and other unsigned integer types
#include <stdio.h> // for printf (debugging)                    
#include <stdlib.h> // for malloc, calloc, free                   
#include "configurations/include.h"
#include "hash.h"
#include "KnownDigests.h"
#include "IntrospectionLibrary/IntrospectionLibrary.c"

void introspective_measurement__init(void)
{
    printf("DEBUG: Here's the result of ShaTest()\n");
    ShaTest();
}

bool introspective_measurement_request(int id, char** evidence)
{
    printf("DEBUG: here's your measurement id: %d\n", id);
    if(id==0)
    {
        EvidenceBundle* resultsBundle = MeasureLinuxKernel();
        int resultsNum = GetCollectionLength(resultsBundle, 100); //TODO find a better supremum
        ExportToByteString(resultsBundle, resultsNum, evidence);
        /* PrintCollection(resultsBundle, resultsNum); */
        free(resultsBundle);
        /* printf("doing with %d\n\n", resultsNum); */
        /* PrintCollection(((EvidenceBundle*)(*evidence)), 3); */
        return true;
    }
    else
    {
        printf("That measurement id does not exist.\n");
        return false;
    }
    return false;
}

bool introspective_measurement_appraise(int id, const char* evidence, char** appraisal_report)
{
    printf("DEBUG: here's your measurement id: %d\n", id);
    if(id==0)
    {
        if(AppraiseLinuxKernelMeasurement(evidence))
        {
            *appraisal_report = strdup("PASS: All Evidence Digests Recognized\0");
        }
        else
        {
            *appraisal_report = strdup("FAIL: Some Evidence Digests NOT Recognized\0");
        }
        return true;
    }
    else
    {
        printf("That measurement id does not exist.\n");
        *appraisal_report = strdup("FAIL: The requested measurement ID does not exist\0");
        return false;
    }
}

