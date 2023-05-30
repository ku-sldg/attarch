/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <camkes.h>
#include "configurations/include.h"
#include "hash.h"
#include "IntrospectionLibrary/IntrospectionLibrary.c"

bool IsModulesOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES] = calloc(NUM_MODULE_DIGESTS, DIGEST_NUM_BYTES);
    char (*module_names)[NUM_MODULE_DIGESTS * INTRO_MODULE_NAME_LEN] = calloc(NUM_MODULE_DIGESTS, INTRO_MODULE_NAME_LEN);
    MeasureKernelModules(memory_device, module_digests, module_names);

    printf("DEBUG: Measurement: Appraising digests\n");
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        if(IsThisAValidModuleMeasurement((char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN])))
        {
            if(IsThisAKnownDigest((uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES])) )
            {
                printf("Module %s recognized:\n", (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]));
            }
            else
            {
                printf("Be warned! Module %s NOT recognized:\n", (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]));
                result = false;
            }
            //mike
            //RenderDigestDeclaration( &((*module_names)[i*INTRO_MODULE_NAME_LEN]) , &((*module_digests)[i*DIGEST_NUM_BYTES]) );

            //gpt4
            RenderDigestDeclaration( (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]) , (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES]) );

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
    char actualArray[INTRO_MODULE_NAME_LEN] = "KernelRodata";
    char (*rodataName)[INTRO_MODULE_NAME_LEN] = &actualArray;
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

        bool modules_appraisal = IsModulesOkay(memdev);
        bool rodata_appraisal = IsKernelRodataOkay(memdev);
        //bool tasks_appraisal =  IsTasksOkay(memdev);

        bool overall_appraisal = true;
        overall_appraisal &= modules_appraisal;
        overall_appraisal &= rodata_appraisal;
        //overall_appraisal &= tasks_appraisal;
        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", overall_appraisal ? "Passed" : "Failed.");
        char* resultMsg = overall_appraisal ? "1" : "0";
        // TODO hook the am back up
        /* memset(ms_dp, '0', 4096); */
        /* strcpy(ms_dp, resultMsg); */
        done_emit();
    }
    return 0;
}

