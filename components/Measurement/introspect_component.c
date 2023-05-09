/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <camkes.h>
#include "configurations/include.h"
#include "hash.h"
#include "IntrospectionLibrary/IL_library.c"

bool IsModulesOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES] = calloc(NUM_MODULE_DIGESTS, DIGEST_NUM_BYTES);
    char (*module_names)[NUM_MODULE_DIGESTS * MODULE_NAME_LEN] = calloc(NUM_MODULE_DIGESTS, MODULE_NAME_LEN);
    MeasureKernelModules(memory_device, module_digests, module_names);

    printf("DEBUG: Measurement: Appraising digests\n");
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        if(IsThisAValidModuleMeasurement((char (*) [MODULE_NAME_LEN])&((*module_names)[i*MODULE_NAME_LEN])))
        {
            if(IsThisAKnownDigest((uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES])) )
            {
                printf("Module %s recognized:\n", (char (*) [MODULE_NAME_LEN])&((*module_names)[i*MODULE_NAME_LEN]));
            }
            else
            {
                printf("Be warned! Module %s NOT recognized:\n", (char (*) [MODULE_NAME_LEN])&((*module_names)[i*MODULE_NAME_LEN]));
                result = false;
            }
            //mike
            //RenderDigestDeclaration( &((*module_names)[i*MODULE_NAME_LEN]) , &((*module_digests)[i*DIGEST_NUM_BYTES]) );

            //gpt4
            RenderDigestDeclaration( (char (*) [MODULE_NAME_LEN])&((*module_names)[i*MODULE_NAME_LEN]) , (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES]) );

        }
    }
    free(module_digests);
    free(module_names);
    return result;
}

bool IsTasksOkay(uint8_t* memory_device)
{
    TaskMeasurement* rootTaskMeasurement = MeasureTaskTree(memory_device);
    bool result = AppraiseTaskTree(rootTaskMeasurement);
    FreeTaskTree(rootTaskMeasurement);
    return result;
}

bool IsKernelRodataOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*kernelRodataDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodata(memory_device, kernelRodataDigest);
    if(IsThisAKnownDigest(kernelRodataDigest))
    {
        printf("Kernel Rodata recognized\n");
    }
    else
    {
        printf("Be warned! Kernel Rodata NOT recognized:\n");
        result = false;
    }
    char actualArray[MODULE_NAME_LEN] = "KernelRodata";
    char (*rodataName)[MODULE_NAME_LEN] = &actualArray;
    RenderDigestDeclaration(rodataName, kernelRodataDigest);
    free(kernelRodataDigest);
    return result;
}

int run(void)
{
    ShaTest();

    // Execution time without introspection: 0.66s
    // Time to Measure Modules: 0.01s
    // Time to Measure Tasks: 0.86s
    // Time to Measure Kernel Rodata: 0.53s
    while (1)
    {
        printf("DEBUG: Measurement: Waiting.\n");
        ready_wait();

        bool appraisal = true;
        appraisal = appraisal && IsModulesOkay(memdev);
        appraisal = appraisal && IsTasksOkay(memdev);
        appraisal = appraisal && IsKernelRodataOkay(memdev);
        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", appraisal ? "Passed" : "Failed.");
        char* resultMsg = appraisal ? "1" : "0";
        // TODO hook the am back up
        /* memset(ms_dp, '0', 4096); */
        /* strcpy(ms_dp, resultMsg); */
        done_emit();
    }
    return 0;
}

