/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <camkes.h>
#include "hash.h"
#include "IntrospectionLibrary/IL_library.c"

#define NUM_MODULE_DIGESTS 128


bool IsModulesOkay()
{
    bool result = true;
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
                result = false;
            }
            RenderDigestDeclaration(module_names+i*56, module_digests+i*64);
        }
    }
    free(module_digests);
    free(module_names);
    return result;
}

bool IsTasksOkay()
{
    TaskMeasurement* rootTaskMeasurement = MeasureTaskTree();
    bool result = AppraiseTaskTree(rootTaskMeasurement);
    FreeTaskTree(rootTaskMeasurement);
    return result;
}

bool IsKernelRodataOkay()
{
    bool result = true;
    uint8_t* kernelRodataDigest = calloc(1, 64);
    MeasureKernelRodata(kernelRodataDigest);
    if(IsThisAKnownDigest(kernelRodataDigest))
    {
        printf("Kernel Rodata recognized\n");
    }
    else
    {
        printf("Be warned! Kernel Rodata NOT recognized:\n");
        result = false;
    }
    RenderDigestDeclaration("KernelRodata", kernelRodataDigest);
    free(kernelRodataDigest);
    return result;
}

int run(void)
{
    ShaTest();

    // Execution time without introspection: 0.67s
    // Time to Measure Modules: 0.03s
    // Time to Measure Tasks: 0.9s
    // Time to Measure Kernel Rodata: 0.55s
    // This whole introspection procedure takes 1.43 seconds.
    while (1)
    {
        printf("DEBUG: Measurement: Waiting.\n");
        ready_wait();
        bool appraisal = true;
        appraisal = appraisal && IsModulesOkay();
        appraisal = appraisal && IsTasksOkay();
        appraisal = appraisal && IsKernelRodataOkay();
        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", appraisal ? "Passed" : "Failed.");
        char* resultMsg = appraisal ? "1" : "0";
        memset(ms_dp, '0', 4096);
        strcpy(ms_dp, resultMsg);
        done_emit();
    }
    return 0;
}

