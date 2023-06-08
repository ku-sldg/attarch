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

    printf("DEBUG: Measurement: Appraising modules\n");
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
    printf("DEBUG: Measurement: Appraising tasks\n");
    bool result = AppraiseTaskTree(rootTaskMeasurement);
    FreeTaskTree(rootTaskMeasurement);
    return result;
}

bool IsKernelRodataOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*kernelRodataDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodata(memory_device, kernelRodataDigest);
    printf("DEBUG: Measurement: Appraising kernel rodata\n");
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



/* #include <stdint.h> */
/* #include <stdlib.h> */

/* size_t* find_potential_pgd(uint8_t* list, size_t size, size_t* num_found) { */
/*     *num_found = 0; */

/*     // Every entry is 8 bytes long, so we'll have at most size / 8 potential entries. */
/*     size_t* offsets = (size_t*)malloc((size / 8) * sizeof(size_t)); */
/*     if (offsets == NULL) { */
/*         return NULL; // Failed to allocate memory. */
/*     } */

/*     size_t potential_entries = 0; */
/*     for (size_t i = 0; i < size - 7; i += 8) { */
/*         uint64_t* potential_pgd = (uint64_t*)(list + i); */

/*         // Simple heuristic: a valid entry is 8 bytes long, the valid bit is set, */
/*         // and it points to an aligned address (we'll check for alignment to a 4KB page). */
/*         if ((*potential_pgd & 0x1) != 0 && (*potential_pgd & 0xFFF000000000) != 0) { */
/*             potential_entries++; */
/*             if (potential_entries >= 256) { */
/*                 offsets[*num_found] = i - (255 * 8); */
/*                 (*num_found)++; */
/*             } */
/*         } else { */
/*             potential_entries = 0; // Reset the count of potential entries. */
/*         } */
/*     } */

/*     return offsets; */
/* } */


void print_byte_list(uint64_t *byte_list, int number_of_bytes_to_read) {
    int i;
    int number_of_chunks = number_of_bytes_to_read / sizeof(uint64_t);  // 8 bytes in a chunk
    for (i = 0; i < number_of_chunks; i++) {
        uint64_t chunk = byte_list[i] & 0x00000000FFFFF000;

        bool isValidAddress = 0 < chunk && ((chunk - RAM_BASE) < RAM_SIZE);
        if(!isValidAddress)
        {
            continue;
        }
        printf("%016X  %s  %X\n", chunk, isValidAddress ? "valid" : "invalid", i*8);
    }
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


        /* print_byte_list(memdev, 8096 * 64 * 2); */


        bool rodata_appraisal = IsKernelRodataOkay(memdev);
        /* bool modules_appraisal = IsModulesOkay(memdev); */
        //bool tasks_appraisal =  IsTasksOkay(memdev);

        bool overall_appraisal = true;
        overall_appraisal &= rodata_appraisal;
        //overall_appraisal &= modules_appraisal;
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

