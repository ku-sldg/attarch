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

// TODO collect this list_head_address as part of the build process
#define LIST_HEAD_ADDR 0xFB61E0

void Sha512(uint8_t* inputBytes, uint32_t inputLength, uint8_t* outputBytes)
{
    // hacl implementation
    Hacl_Hash_SHA2_hash_512(inputBytes, inputLength, outputBytes);
}

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
        printf("DEBUG: Measurement: Beginning measurement.\n");

        debugPrint("Collecting module pointers...\n");
        /* modulePtrs is a list of offsets into memdev that refer to kernel
        ** modules. They are physical memory addresses with the RAM_BASE
        ** already subtracted. Assume there are no more than 128 modules.
        */
        uint64_t modulePtrs[128];
        for(int i=0; i<128; i++)
        {
            modulePtrs[i] = 0;
        }
        int numModulePtrs = 0;
        uint64_t* list_head_ptr = (uint64_t*)(((char*)memdev)+LIST_HEAD_ADDR);
        uint64_t module_pointer = TranslationTableWalk(list_head_ptr[0]);
        while(module_pointer != LIST_HEAD_ADDR)
        {
            modulePtrs[numModulePtrs] = module_pointer;
            numModulePtrs++;
            char* modBytePtr = ((char*)memdev)+module_pointer;
            uint64_t* modLongPtr = (uint64_t*)modBytePtr;
            module_pointer = TranslationTableWalk(modLongPtr[0]);
        }

        debugPrint("Collecting digests over module rodata...\n");
        uint8_t** module_digests = calloc(128, sizeof(uint8_t*));
        char** module_names = calloc(128, sizeof(char*));
        bool* module_appraisal_results = calloc(128, sizeof(bool));
        for(int i=0; i<128; i++)
        {
            module_digests[i] = calloc(1, 64 * sizeof(uint8_t));
            module_names[i] = calloc(1, 56 * sizeof(char));
            module_appraisal_results[i] = false;
        }

        //debug
        bool IsThisAValidModuleMeasurement(char* moduleName)
        {
            // just need to check if this is the empty string
            // <=> the first char is the 0 byte
            /*
            if(moduleName[0] == '/0')
            {
                return false;
            }
            return true;
            */

            for(int i=0; i<56; i++)
            {
                if(moduleName[i] != '\0')
                {
                    // an invalid (unused) module name should be completely
                    // zeroed out
                    return true;
                }
            }
            return false;
        }

        for(int i=0; i<128; i++)
        {
            if(modulePtrs[i] != 0)
            {
                InterpretKernelModule(modulePtrs[i], module_digests[i], module_names[i]);
            }
        }

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

        done_emit();
    }

    return 0;
}

