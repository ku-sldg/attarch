/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 09 June 2023
 */

#include <camkes.h>
#include "configurations/include.h"
#include "hash.h"
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
        printf("\n\nbundle is\n\n");
        PrintCollection(resultsBundle, resultsNum);
        ExportToByteString(resultsBundle, resultsNum, evidence);
        printf("\n\nbytestring bundle is\n\n");
        PrintCollection(*evidence, resultsNum);
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
    printf("DEBUG: here's your evidence:\n");
    int resultsNum = GetCollectionLength(evidence, 100); //TODO find a better supremum
    PrintCollection(evidence, resultsNum);
    if(id==0)
    {
        /* bool result = MeasureAndAppraiseLinux(); */
        printf("\n\nDummy Appraisal\n\n");
        bool result = true;

        *appraisal_report = malloc(30);
        for(int i=0; i<30; i++)
        {
            (*appraisal_report)[i] = 'a';
        }
        (*appraisal_report)[29] = '\0';

        return true;
    }
    else
    {
        printf("That measurement id does not exist.\n");
        return false;
    }
}

