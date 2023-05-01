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

uint32_t fib_buf[10];

int run(void)
{
    ShaTest();


    while (1) {
        ready_wait();
        printf("introspect: Got an event\n");

        seL4_Word paddr = *(seL4_Word *)introspect_data;
        printf("paddr in component 0x%x\n", paddr);

        seL4_Word offset = paddr - RAM_BASE;

        printf("offset in component 0x%x\n", offset);

        memcpy(fib_buf, ((char *)memdev + offset), sizeof(uint32_t) * 10);

        for (int i = 0; i < 10; i++) {
            printf("camkes_fib[%d]@%p = %d, ", i, (fib_buf + i), fib_buf[i]);
        }

        printf("\n");
        done_emit();
    }



    /*
    printf("\n\n\nDoing scanning tests...\n\n\n");
    for(int i=0; i<RAM_SIZE; i++)
    {
        uint8_t thisChar = ((char*)memdev)[i];
        if(thisChar == 0)
        {
            continue;
        }
        if( 0 < thisChar && thisChar < 128 )
        {
            printf("%c",((char*)memdev)[i]);
        }
        else
        {
            printf("%02X",((char*)memdev)[i]);
        }
    }
    printf("\n\n\nEnding scanning tests...\n\n\n");
    */

    // Execution time without introspection: 0.66s
    // Time to Measure Modules: 0.01s
    // Time to Measure Tasks: 0.86s
    // Time to Measure Kernel Rodata: 0.53s
    /* commented out 1 May 2023
    while (1)
    {
        printf("DEBUG: Measurement: Waiting.\n");
        //measurement_request_wait();
        ready_wait();
        bool appraisal = true;
        appraisal = appraisal && IsModulesOkay();
        appraisal = appraisal && IsTasksOkay();
        appraisal = appraisal && IsKernelRodataOkay();
        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", appraisal ? "Passed" : "Failed.");
        char* resultMsg = appraisal ? "1" : "0";
        // TODO hook the am back up
        //memset(ms_dp, '0', 4096);
        //strcpy(ms_dp, resultMsg);
        //measurement_done_emit();
        done_emit();
    }
    */
    return 0;
}

