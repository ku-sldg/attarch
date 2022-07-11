/*
 * An introspection library for Linux 4.9.307
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#define RAM_BASE 0x40000000

bool debug = false;
void debugPrint(char* msg)
{
    if(debug){printf(msg);}
}

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

    if(debug)
    {
        printf("input %016X,\nPGDindex %016X,\nPUDindex %016X,\nPMDindex %016X,\nPTEindex %016X\n", inputAddr, PGDindex, PUDindex, PMDindex, PTEindex); 
        printf("PGDindex %d,\nPUDindex %d,\nPMDindex %d,\nPTEindex %X\n", PGDindex, PUDindex, PMDindex, PTEindex); 
    }
    char* PGDTablePtr = ((char*)memdev)+0x4113D000 - RAM_BASE;
    uint64_t* PGDTable = (uint64_t*)PGDTablePtr;
    uint64_t pudAddr = (PGDTable[PGDindex] & 0x00000000FFFFF000) - RAM_BASE;

    if(debug)
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

    if(debug)
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

    if(debug)
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

    if(debug)
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

void InterpretKernelModule(uint64_t inputAddress, uint8_t* rodataDigest, char* name)
{
    if(debug)
    {
        printf("Module Address: %016X\n", inputAddress);
    }

    for(int j=16; j<56+16; j++)
    {
        name[j-16] = ((char*)memdev)[inputAddress+j];
    }

    printf("DEBUG: Measurement: Found Module: ");
    for(int j=0; j<56; j++)
    {
        printf("%c", name[j]);
    }
    printf("\n");

    struct module_layout thisModuleLayout = GetModuleLayoutFromListHead((int)inputAddress);
    uint64_t basePtr = TranslationTableWalk(thisModuleLayout.base);

    if(debug)
    {
        printf("base: %016X\n", thisModuleLayout.base);
        printf("size: %08X\n", thisModuleLayout.size);
        printf("text size: %08X\n", thisModuleLayout.text_size);
        printf("ro size: %08X\n", thisModuleLayout.ro_size);
        printf("ro after init size: %08X\n", thisModuleLayout.ro_after_init_size);
        printf("base paddr: %016X\n", basePtr);
    }

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

