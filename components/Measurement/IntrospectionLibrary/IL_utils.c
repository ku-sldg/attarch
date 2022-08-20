/*
 * An introspection library for Linux 4.9.307
 * General Utilities
 * Michael Neises
 * 21 July 2022
 */

uint64_t intro_virt_to_phys(uint64_t virtaddr)
{
    //return((virtaddr & 0xFFFFFFFF) + 0x40000000);
    return(virtaddr & 0xFFFFFFFF);
}

// pgd should be the physical address of the page global directory structure
uint64_t TranslationTableWalkSuppliedPGD(uint64_t inputAddr, uint64_t pgd)
{
    bool TTWalkDebug = false;

    uint64_t PGDindex = (inputAddr & 0x0000FF8000000000) >> 39;
    uint64_t PUDindex = (inputAddr & 0x0000007FC0000000) >> 30;
    uint64_t PMDindex = (inputAddr & 0x000000003FE00000) >> 21;
    uint64_t PTEindex = (inputAddr & 0x00000000001FF000) >> 12;
    uint64_t PAGindex = (inputAddr & 0x0000000000000FFF) >>  0;

    if(TTWalkDebug)
    {
        printf("input %016X,\nPGDindex %016X,\nPUDindex %016X,\nPMDindex %016X,\nPTEindex %016X\n", inputAddr, PGDindex, PUDindex, PMDindex, PTEindex); 
        printf("PGDindex %d,\nPUDindex %d,\nPMDindex %d,\nPTEindex %X\n", PGDindex, PUDindex, PMDindex, PTEindex); 
    }
    char* PGDTablePtr = ((char*)memdev)+pgd;
    uint64_t* PGDTable = (uint64_t*)PGDTablePtr;
    uint64_t pudAddr = (PGDTable[PGDindex] & 0x00000000FFFFF000) - RAM_BASE;
    //uint64_t pudAddr = pgd;

    if(TTWalkDebug)
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

    if(TTWalkDebug)
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

    if(TTWalkDebug)
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

    if(TTWalkDebug)
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

uint64_t TranslationTableWalk(uint64_t inputAddr)
{
    /* char* PGDTablePtr = ((char*)memdev)+0x4113D000 - RAM_BASE; */
    char* PGDTablePtr = 0x4113D000 - RAM_BASE;
    /* uint64_t PGDTable = ((uint64_t*)PGDTablePtr)[0]; */
    return TranslationTableWalkSuppliedPGD(inputAddr,  PGDTablePtr);
}

uint64_t TranslateVaddr(uint64_t vaddr)
{
    //TODO I don't think this conditional is right
    // but the funtion never dies terribly
    if( 0xffff800000000000 <= vaddr && vaddr <= 0xffff800008001000 )
    {
        return intro_virt_to_phys(vaddr);
    }
    return TranslationTableWalk(vaddr);
}









void introLog(int args, ...)
{
    if(true)
    {
        printf("DEBUG: Introspection: ");
        va_list ptr;
        va_start(ptr, args);
        for(int i=0; i<args; i++)
        {
            printf("%s", va_arg(ptr,char*));
        }
    }
}

void introspectScan(int* head, int size, char* name)
{
    printf("%s: ", name);
    //for(int i=0; i<size; i++)
    for(int i=size-1; i>=0; i--)
    {
        printf("%02X",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanChar(int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    //for(int i=size; i>0; i--)
    {
        printf("%c",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanInt(int* head, char* name)
{
    printf("%s: ", name);
    int* val = (int*)((char*)memdev+*head);
    printf("%d : %u", *val, *val);
    printf("\n");
    *head += 4;
}
void introspectScanLong(int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memdev+*head);
    printf("%lu", *val);
    printf("\n");
    *head += 8;
}
void introspectScanAddr(int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memdev+*head);
    printf("%p : %016X", *val, *val);
    printf("\n");
    *head += 8;
}

void introspectScanMaybeChar(int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    {
        uint8_t thisChar = ((char*)memdev+*head)[i];
        if( 0 < thisChar && thisChar < 128 )
        {
            printf("%c",((char*)memdev+*head)[i]);
        }
        else
        {
            printf("%02X",((char*)memdev+*head)[i]);
        }
    }
    printf("\n");
    *head += size;
}

