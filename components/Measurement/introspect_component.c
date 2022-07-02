/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include <camkes.h>
#include <Hacl_Hash_SHA2.h>

#define RAM_BASE 0x40000000
#define N 10
#define LIST_HEAD_ADDR 0xFB61E0
uint32_t fib_buf[N];

struct list_head {
    struct list_head *next, *prev;
};

uint64_t TranslationTableWalk(uint64_t inputAddr)
{
    bool isDebugLog = false;
    uint64_t PGDindex = (inputAddr & 0x0000FF8000000000) >> 39;
    uint64_t PUDindex = (inputAddr & 0x0000007FC0000000) >> 30;
    uint64_t PMDindex = (inputAddr & 0x000000003FE00000) >> 21;
    uint64_t PTEindex = (inputAddr & 0x00000000001FF000) >> 12;
    uint64_t PAGindex = (inputAddr & 0x0000000000000FFF) >>  0;

    if(isDebugLog)
    {
        printf("input %016X,\nPGDindex %016X,\nPUDindex %016X,\nPMDindex %016X,\nPTEindex %016X\n", inputAddr, PGDindex, PUDindex, PMDindex, PTEindex); 
        printf("PGDindex %d,\nPUDindex %d,\nPMDindex %d,\nPTEindex %X\n", PGDindex, PUDindex, PMDindex, PTEindex); 
    }
    char* PGDTablePtr = ((char*)memdev)+0x4113D000 - RAM_BASE;
    uint64_t* PGDTable = (uint64_t*)PGDTablePtr;
    uint64_t pudAddr = (PGDTable[PGDindex] & 0x00000000FFFFF000) - RAM_BASE;
    if(isDebugLog)
    {
        printf("Here is the PGD\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%016X\n", PGDTable[i]);
        }
        printf("Get PUD base address from PGD\n");
        printf("pudAddr is %016X\n", pudAddr);
    }
    // TODO investigate these bits we drop from every table entry
    char* pudTablePtr = ((char*)memdev)+pudAddr;
    uint64_t* PUDTable = (uint64_t*)pudTablePtr;
    uint64_t pmdAddr = (PUDTable[PUDindex] & 0x00000000FFFFF000) - RAM_BASE;
    if(isDebugLog)
    {
        printf("Here is the PUD\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%X: %016X\n", i, PUDTable[i]);
        }
        printf("pmdAddr is %016X\n", pmdAddr);
    }
    char* pmdTablePtr = ((char*)memdev)+pmdAddr;
    uint64_t* pmdTable = (uint64_t*)pmdTablePtr;
    uint64_t pteAddr = (pmdTable[PMDindex] & 0x00000000FFFFF000) - RAM_BASE;
    if(isDebugLog)
    {
        printf("Here is the pmd\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%X: %016X\n", i, pmdTable[i]);
        }
        printf("pteAddr is %016X\n", pteAddr);
    }
    char* pteTablePtr = ((char*)memdev)+pteAddr;
    uint64_t* pteTable = (uint64_t*)pteTablePtr;
    uint64_t offsetAddr = (pteTable[PTEindex] & 0x00000000FFFFF000) - RAM_BASE;
    uint64_t finalPaddr = offsetAddr | PAGindex;
    if(isDebugLog)
    {
        printf("Here is the pte at 1C2\n");
        for(int i=0x1C2; i<0x1C6; i++)
        {
            printf("%X: %016X\n", i, pteTable[i]);
        }
        printf("offsetAddr is %016X\n", offsetAddr);
        printf("Output Address is %016X\n", finalPaddr + RAM_BASE);
        printf("\nTable walk complete\n");
    }
    return finalPaddr;
}

struct module_layout {
    uint64_t base;
    unsigned int size;
    unsigned int text_size;
    unsigned int ro_size;
    unsigned int ro_after_init_size;
};

struct module_layout GetModuleLayoutFromListHead(int physAddr)
{
    int index = physAddr;
    index += 16; // skip list_head
    index += 56; // skip name
    index += 96; //skip mkobj
    index += 8; // skio modinfo_attrs
    index += 8; // skip version
    index += 8; // skip srcversion
    index += 8; // skip holders_dir
    index += 8; // skip syms
    index += 8; // skip crcs
    index += 4; // skip num_syms
    index += 40; // skip struct mutex
    index += 8; // skip kp
    index += 4; // num_kp
    index += 4; // num_gpl_syms
    index += 8; // gpl_syms
    index += 8; // gpl_crcs
    index += 1; //async_probe_requested
    index += 8; // gpl_future_syms
    index += 8; // gpl_future_crcs
    index += 4; // num_gpl_future_syms
    index += 4; // num_exentries
    index += 8; // extable
    index += 8; // (*init*(void)

    // a correction ?
    index += 3;

    //by inspection
    //printerate(physAddr + 47 * 8, 3);
    struct module_layout thisModule;
    thisModule.base = ((uint64_t*)((char*)memdev+physAddr+47*8))[0];
    thisModule.size = ((unsigned int*)((char*)memdev+physAddr+47*8))[2];
    thisModule.text_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[3];
    thisModule.ro_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[4];
    thisModule.ro_after_init_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[5];
    return thisModule;
}

struct module_measurement {
    char module_name[56];
    uint8_t* ro_data;
};

void Sha512(uint8_t* inputBytes, uint32_t inputLength, uint8_t* outputBytes)
{
    // hacl implementation
    Hacl_Hash_SHA2_hash_512(inputBytes, inputLength, outputBytes);

    /* sph lib
    sph_sha512_context shaContext;
    sph_sha512_context* contextPtr = &shaContext;
    sph_sha512_init((void*)contextPtr);
    void* input = (void*)inputBytes;
    sph_sha512((void*)contextPtr, input, inputLength);
    void* output = malloc(64);
    sph_sha512_close((void*)contextPtr, output);
    for(int i=0; i<64; i++)
    {
        outputBytes[i] = ((uint8_t*)output)[i];
    }
    free(output);
    */
    /*
    printf("output is:\n", outputBytes);
    for(int i=0; i<64; i++)
    {
        printf("%02X ", outputBytes[i]);
    }
    printf("\n");
    */
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

void InterpretKernelModule(uint64_t inputAddress, uint8_t* rodataDigest)
{
    //printf("Module Address: %016X\n", inputAddress);
    //printf("top. module_pointer is %016X\n", module_pointer);
    /*
    for(int j=0; j<24; j++)
    {
        if(j%8==0&&j>0){printf("\n");}
        if(j<16){printf("%02X", ((char*)memdev)[inputAddress + j]);}
        else{printf("%C", ((char*)memdev)[inputAddress + j]);}
    }
    printf("\n");
    */

    char module_name[56];
    for(int j=16; j<56+16; j++)
    {
        module_name[j-16] = ((char*)memdev)[inputAddress+j];
    }
    /*
    printf("Module Name: ");
    for(int j=0; j<56; j++)
    {
        printf("%c", module_name[j]);
    }
    printf("\n");
    */

    struct module_layout thisModuleLayout = GetModuleLayoutFromListHead((int)inputAddress);
    uint64_t basePtr = TranslationTableWalk(thisModuleLayout.base);
    /*
    printf("base: %016X\n", thisModuleLayout.base);
    printf("size: %08X\n", thisModuleLayout.size);
    printf("text size: %08X\n", thisModuleLayout.text_size);
    printf("ro size: %08X\n", thisModuleLayout.ro_size);
    printf("ro after init size: %08X\n", thisModuleLayout.ro_after_init_size);
    printf("base paddr: %016X\n", basePtr);
    */
    // collect the read-only data
    uint8_t* rodata = malloc(thisModuleLayout.ro_size);
    for(int i=0; i<thisModuleLayout.ro_size; i++)
    {
        rodata[i] = ((char*)memdev)[basePtr+i];
    }

    // digest the read-only data with Sha512 from sphlib
    Sha512(rodata, thisModuleLayout.ro_size, rodataDigest);
    free(rodata);

    // can print out the rodata here to see strings from the source
    //printerateChars(basePtr, thisModuleLayout.ro_size);
}

void printerate(int physAddr, int numLongs)
{
    uint64_t* printerHead = (uint64_t*)((char*)memdev+physAddr);
    for(int i=0; 8*i < numLongs; i++)
    {
        for(int j=0; j<8; j++)
        {
            printf("%016X ", printerHead[i*8+j]);
            //printf("%02X", ((char*)memdev)[physAddr + 8*i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printerateChars(int physAddr, int numBytes)
{
    char* printerHead = ((char*)memdev+physAddr);
    for(int i=0; i < numBytes; i++)
    {
        if(i%64==0&&i!=0){printf("\n");}
        printf("%c ", printerHead[i]);
    }
    printf("\n");
}


void ShaTest()
{
    printf("The Sha512 digest of 'abc' is:\n");
    uint8_t* output = malloc(64);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    PrintDigest(output);
    /* sph lib
    sph_sha512_context shaContext;
    sph_sha512_context* contextPtr = &shaContext;
    sph_sha512_init((void*)contextPtr);
    char* charinput = malloc(3);
    charinput = "abc";
    void* input = (void*)charinput;
    size_t len = 3;
    sph_sha384((void*)contextPtr, input, len);
    void* output = malloc(64);
    sph_sha512_close((void*)contextPtr, output);
    uint8_t* byteOutput = (uint8_t*)output;
    PrintDigest(byteOutput);
    */
}

int run(void)
{
    while (1) {
        ready_wait();
        ShaTest();
        printf("introspect: Got an event\n");

        printf("Collecting module pointers...\n");
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

        printf("Collecting digests over module rodata...\n");
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

        printf("Digests collected:\n");
        for(int i=0; i<4; i++)
        {
            PrintDigest(module_digests+i*64);
            printf("\n");
        }

        done_emit();
    }

    return 0;
}

