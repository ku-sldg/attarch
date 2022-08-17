/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <camkes.h>
#include "hash.h"
#include "IntrospectionLibrary/IL_library.c"

#define NUM_MODULE_DIGESTS 128

int run(void)
{
    ShaTest();
    while (1)
    {
        printf("DEBUG: Measurement: Waiting.\n");
        ready_wait();
        
        bool ultimateAppraisalResult = true;

        uint8_t* module_digests = calloc(NUM_MODULE_DIGESTS, 64);
        char* module_names = calloc(NUM_MODULE_DIGESTS, 56);
        MeasureKernelModules(module_digests, module_names);

        printf("DEBUG: Measurement: Appraising digests\n");
        for(int i=0; i<NUM_MODULE_DIGESTS; i++)
        {
            if(IsThisAValidModuleMeasurement(module_names+i*56))
            {
                if(IsThisAKnownDigest(module_digests+i*64))
                {
                    printf("Module %s recognized:\n", module_names+i*56);
                }
                else
                {
                    printf("Be warned! Module %s NOT recognized:\n", module_names+i*56);
                    ultimateAppraisalResult = false;
                }
                RenderDigestDeclaration(module_names+i*56, module_digests+i*64);
            }
        }

        /* Measure Running Processes */
        TaskMeasurement* rootTaskMeasurement = MeasureTaskTree();
        bool taskTreeResult = AppraiseTaskTree(rootTaskMeasurement);
        ultimateAppraisalResult = ultimateAppraisalResult && taskTreeResult;
        FreeTaskTree(rootTaskMeasurement);

        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", ultimateAppraisalResult ? "Passed" : "Failed.");

        char* resultMsg = ultimateAppraisalResult ? "1" : "0";
        memset(ms_dp, '0', 4096);
        strcpy(ms_dp, resultMsg);

        done_emit();
    }

    return 0;
}

