/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <Hacl_Hash.h>
#include "introspection_library.c"
#include "appraisal.c"

void PrintDigest(uint8_t* digest, char* name)
{
    printf("Module Name: %s\nModule Rodata Digest:\n", name);
    for(int i=0; i<64; i++)
    {
        //if(i>0&&i%16==0){printf("\n");}
        printf("%02X", digest[i]);
    }
    printf("\n");
}

void ShaTest()
{
    uint8_t* output = malloc(64);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    PrintDigest(output, "abc");
}

int run(void)
{
    ShaTest();
    while (1)
    {
        printf("DEBUG: Measurement: Waiting.\n");
        ready_wait();

        uint8_t** module_digests = calloc(128, sizeof(uint8_t*));
        char** module_names = calloc(128, sizeof(char*));
        for(int i=0; i<128; i++)
        {
            module_digests[i] = calloc(1, 64 * sizeof(uint8_t));
            module_names[i] = calloc(1, 56 * sizeof(char));
        }
        MeasureKernelModules(module_digests, module_names);

        printf("DEBUG: Measurement: Presenting evidence\n");
        for(int i=0; i<128; i++)
        {
            if(IsThisAValidModuleMeasurement(module_names[i]))
            {
                PrintDigest(module_digests[i], module_names[i]);
                printf("\n");
            }
        }

        printf("DEBUG: Measurement: Appraising digests\n");
        bool* module_appraisal_results = calloc(128, sizeof(bool));
        for(int i=0; i<128; i++)
        {
            if(IsThisAValidModuleMeasurement(module_names[i]))
            {
                AppraiseKernelModule(module_digests[i], &module_appraisal_results[i], module_names[i]);
            }
        }

        char** pass_module_names = calloc(128, sizeof(char*));
        for(int i=0; i<128; i++)
        {
            pass_module_names[i] = calloc(56, sizeof(char));
        }
        for(int i=0; i<128; i++)
        {
            if(IsThisAValidModuleMeasurement(module_names[i]))
            {
                if(module_appraisal_results[i])
                {
                    pass_module_names[i] = module_names[i];
                }
            }
        }

        bool ultimateAppraisalResult = true;
        for(int i=0; i<128; i++)
        {
            if(IsThisAValidModuleMeasurement(pass_module_names[i]))
            {
                printf("DEBUG: Measurement: %s module: Appraisal Passed.\n", pass_module_names[i]);
            }
            else if(IsThisAValidModuleMeasurement(module_names[i]))
            {
                ultimateAppraisalResult = false;
                printf("DEBUG: Measurement: %s module: Appraisal Failed.\n", module_names[i]);
            }
        }
        printf("DEBUG: Measurement: Overall Appraisal Result: %s\n", ultimateAppraisalResult ? "Passed" : "Failed.");

        char* resultMsg = ultimateAppraisalResult ? "1" : "0";
        memset(ms_dp, '0', 4096);
        strcpy(ms_dp, resultMsg);


        MeasureProcesses();


        done_emit();
    }

    return 0;
}

