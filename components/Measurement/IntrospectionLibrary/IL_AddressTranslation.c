/*
 * An introspection library for Linux 4.9.307
 * Handles virtual address translation for the following system description:
 * AArch64 Linux system using a 4-level translation table.
 * See the Arm Architecture Reference Manual for A-profile architecture:
 * Chapter D5: The AArch64 Virtual Memory System Architecture
 * Translation Granule Size: 4k
 * Translation Regime: see regime.txt
 * See in particular Section D.5.2.6 Subsection: Overview of VMSAv8-64 address translation using the 4KB translation granule
 * Michael Neises
 * 2 May 2023
 */

uint64_t intro_virt_to_phys(uint64_t virtaddr)
{
    //return((virtaddr & 0xFFFFFFFF) + 0x40000000);
    return(virtaddr & 0xFFFFFFFF);
}

// pgd should be the physical address of the page global directory structure
uint64_t TranslationTableWalkSuppliedPGD(uint8_t* memory_device, uint64_t inputAddr, uint64_t pgd)
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
    char* PGDTablePtr = ((char*)memory_device)+pgd;
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
    char* pudTablePtr = ((char*)memory_device)+pudAddr;
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

    char* pmdTablePtr = ((char*)memory_device)+pmdAddr;
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

    char* pteTablePtr = ((char*)memory_device)+pteAddr;
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

uint64_t TranslationTableWalk(uint8_t* memory_device, uint64_t inputAddr)
{
    char* PGDTablePtr = 0x4113D000 - RAM_BASE;
    return TranslationTableWalkSuppliedPGD(memory_device, inputAddr,  PGDTablePtr);
}

uint64_t TranslateVaddr(uint8_t* memory_device, uint64_t vaddr)
{
    //TODO I don't think this conditional is right
    // but the funtion never dies terribly
    uint64_t vaddr_base = 0xffff800000000000;
    if( vaddr_base <= vaddr && vaddr <= vaddr_base+RAM_SIZE )
    {
        return intro_virt_to_phys(vaddr);
    }
    return TranslationTableWalk(memory_device, vaddr);
}

