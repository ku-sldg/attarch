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


// #define MEM_SV_START        _AC(0xfd000000, UL)
//  OR
// #define KERNEL_HIGH_VADDR   _AC(0xfffffff800000000, UL)  /* high 32GB */
// #define MEM_SV_START        (KERNEL_HIGH_VADDR - 0x100000000) /* 256 MB */

// #define TEXT_OFFSET MEM_SV_START

// linux/arch/arm64/include/asm/kernel-pgtable.h
// #define ARM64_MEMSTART_ALIGN 0x40000000

// BY INSPECTION: (via printing it in a kernel module)
// #define PHYS_OFFSET RAM_BASE
//see linux/arch/arm64/kernel/head.S
// kimage_vaddr = 0xFFFF000004000001
// kimage_voffset = 0xFFFF000004000001- 0x40000000
// kimage_voffset = 0xFFFEFFFFC4000000

// ./include/linux/kernel.h:69:#define __round_mask(x, y) ((__typeof__(x))((y)-1))
// ./include/linux/kernel.h:71:#define round_down(x, y) ((x) & ~__round_mask(x, y))

// see linux/arch/arm64/mm/init.c
//  memstart_addr = round_down(memblock_start_of_DRAM(),
//                     0x40000000);

// see linux/arch/arm64/include/asm/memory.h

// kimage_vaddr := 0xFFFF000004000001
// #define VA_START 0xFFFF000000000001
// #define PHYS_OFFSET memstart_addr
// #define PAGE_OFFSET 0xFFFF800000000001
// #define CONFIG_ARM64_VA_BITS 48

/* uint64_t intro_virt_to_phys_new(uint64_t virtaddr) */
/* { */
/*     uint64_t ret = virtaddr & (1 << 47) ? (virtaddr & ~0xFFFF800000000001) + PHYS_OFFSET : (virtaddr - kimage_voffset); */
/*     return ret; */
/* } */


// this 48 := CONFIG_ARM64_VA_BITS
#define PAGE_OFFSET (-(1ULL << 48))
// this 47 := vabits_actual - 1
#define PAGE_END (-(1ULL << 47))

bool is_linear_map_address(uint64_t vaddr)
{
    return ((vaddr ^ PAGE_OFFSET) < (PAGE_END - PAGE_OFFSET));
}


uint64_t intro_virt_to_phys(uint64_t virtaddr)
{
    uint64_t ret;
    if(is_linear_map_address(virtaddr))
    {
        /* printf("linear marty\n"); */
        ret = ( (virtaddr & ~PAGE_OFFSET) );
    }
    else // this is a bit funny, no?
    {
        /* printf("nonlinear marty\n"); */
        uint64_t kimage_vaddr = 0xFFFF800008000000;
        ret = virtaddr > kimage_vaddr ? (virtaddr - kimage_vaddr) : virtaddr;
    }
    /* printf("virt_to_phys transformed %llx into %llx\n", virtaddr, ret); */
    return ret;
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
        printf("input %016X\nPGDindex %016X\nPUDindex %016X\nPMDindex %016X\nPTEindex %016X\nPAGindex %016X\n", inputAddr, PGDindex, PUDindex, PMDindex, PTEindex, PAGindex); 
        printf("PGDindex %d\nPUDindex %d\nPMDindex %d\nPTEindex %d\nPAGindex %d\n", PGDindex, PUDindex, PMDindex, PTEindex, PAGindex); 
    }
    char* PGDTablePtr = ((char*)memory_device)+pgd;
    uint64_t* PGDTable = (uint64_t*)PGDTablePtr;
    uint64_t pudAddr = (PGDTable[PGDindex] & 0x00000000FFFFF000) - RAM_BASE;
    //uint64_t pudAddr = pgd;
    if(RAM_SIZE < pudAddr)
    {
        return 0;
    }

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
    if(RAM_SIZE < pmdAddr)
    {
        return 0;
    }

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
    if(RAM_SIZE < pteAddr)
    {
        return 0;
    }

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
    if(RAM_SIZE < finalPaddr)
    {
        return 0;
    }

    if(TTWalkDebug)
    {
        printf("offsetAddr is %016X\n", offsetAddr);
        printf("Output Address is %016X\n", finalPaddr + RAM_BASE);
        printf("Table walk complete\n\n");
        printf("Table Walk transformed %llx into %llx\n", inputAddr, finalPaddr);
    }

    /* printf("Table Walk transformed %llx into %llx\n", inputAddr, finalPaddr); */
    return finalPaddr;
}

uint64_t TranslationTableWalk(uint8_t* memory_device, uint64_t inputAddr)
{
    /* printf("translating swapper pgd\n"); */
    uint64_t swapper_pgd_table_paddr = intro_virt_to_phys((uint64_t)INTRO_SWAPPER_PG_DIR_VADDR + 0x80000);
    /* char* PGDTablePtr = (char*)swapper_pgd_table_paddr; */
    /* printf("swapper pgd translated. actually walking table now\n"); */
    return TranslationTableWalkSuppliedPGD(memory_device, inputAddr, swapper_pgd_table_paddr);
}

uint64_t TranslateVaddr(uint8_t* memory_device, uint64_t vaddr)
{
    if( PAGE_OFFSET < vaddr )
    {
        /* printf("passing to virt_to_phys\n"); */
        return intro_virt_to_phys(vaddr);
    }
    /* printf("walking table\n"); */
    return TranslationTableWalk(memory_device, vaddr);
}

