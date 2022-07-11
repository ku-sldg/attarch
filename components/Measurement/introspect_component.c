/*
 * A Measurement and Appraisal component for CAmkES
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <Hacl_Hash.h>
#include "introspection_library.c"

// TODO collect this list_head_address as part of the build process
#define LIST_HEAD_ADDR 0xFB61E0

void Sha512(uint8_t* inputBytes, uint32_t inputLength, uint8_t* outputBytes)
{
    // hacl implementation
    Hacl_Hash_SHA2_hash_512(inputBytes, inputLength, outputBytes);
}

void PrintDigest(uint8_t* digest)
{
    for(int i=0; i<64; i++)
    {
        if(i>0&&i%16==0){printf("\n");}
        printf("%02X ", digest[i]);
    }
    printf("\n");
}

void ShaTest()
{
    printf("Measurement: The Sha512 digest of 'abc' is:\n");
    uint8_t* output = malloc(64);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    PrintDigest(output);
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
        uint8_t* module_digests = malloc(64 * 128);
        for(int i=0; i<64*128; i++)
        {
            module_digests[i] = 0;
        }
        for(int i=0; i<128; i++)
        {
            if(modulePtrs[i] != 0)
            {
                InterpretKernelModule(modulePtrs[i], module_digests+i*64);
            }
        }

        printf("DEBUG: Measurement: Presenting evidence\n");
        for(int i=0; i<4; i++)
        {
            PrintDigest(module_digests+i*64);
            printf("\n");
        }

        done_emit();
    }

    return 0;
}

