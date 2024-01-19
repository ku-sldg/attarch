/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */


#include "../hash.h"


void MeasureKernelRodataBytes(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t startVaddr, uint64_t num_bytes)
{
    uint64_t pagePaddr = TranslateVaddr(memory_device, startVaddr);
    HashMeasure( ((char*)memory_device+pagePaddr), num_bytes, output_digest );
}

void MeasureKernelRodataPage(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    MeasureKernelRodataBytes(memory_device, output_digest, pageVaddr, INTRO_PAGE_SIZE);
}

void MeasureAndPrintPage(uint8_t* memory_device, uint64_t pageVaddr)
{
    uint8_t (*temp_digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodataPage(memory_device, temp_digest, pageVaddr);
    PrintDigest(temp_digest);
    free(temp_digest);
}

void CollectRodataHashingAsWeGo(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    int numPagesMeasured = 0;
    int numBytesMeasured = 0;
    uint64_t RoundUpToNextPage(uint64_t x)
    {
        return ((x + 0xFFF) >> 12) << 12;
    }
    uint64_t RoundDownToCurrentPage(uint64_t x)
    {
        return (x >> 12) << 12;
    }

    uint8_t (*temp_digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray)[NUM_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_RODATA_PAGES, DIGEST_NUM_BYTES);
    int numRodataPagesActuallyMeasured = 0;
    for(int i=0; i<NUM_RODATA_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_START_RODATA_VADDR)+ i * INTRO_PAGE_SIZE;
        //printf("%04X\n", (thisPageVaddr & 0xFFFFFFF) >> 12);
        if(thisPageVaddr >= RoundDownToCurrentPage(INTRO_START_RO_AFTER_INIT_VADDR)
                && thisPageVaddr <= RoundUpToNextPage(INTRO_END_RO_AFTER_INIT_VADDR))
        {
            /* printf("Skipping ro_after_init data.\n"); */
            continue;
        }
        numRodataPagesActuallyMeasured++;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisPageVaddr + 0x80000);
    }
    printf("Measured %d rodata pages between %p and %p\n", numRodataPagesActuallyMeasured, INTRO_START_RODATA_VADDR, INTRO_END_RODATA_VADDR);
    HashMeasure((uint8_t*)digestArray, NUM_RODATA_PAGES * DIGEST_NUM_BYTES, temp_digest);
    free(digestArray);
    HashExtend(output_digest, temp_digest);
    PrintDigest(temp_digest);
    free(temp_digest);
    numPagesMeasured += numRodataPagesActuallyMeasured;

    uint8_t (*temp_digest3)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray3)[NUM_HYP_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_HYP_RODATA_PAGES, DIGEST_NUM_BYTES);
    printf("Measuring %d hyp_rodata pages from %p to %p\n", NUM_HYP_RODATA_PAGES, INTRO_HYP_RODATA_START_VADDR, INTRO_HYP_RODATA_END_VADDR);
    for(int i=0; i<NUM_HYP_RODATA_PAGES; i++)
    {
        if(i==0 || i==2 || i==3)
        {
            // these pages are certainly not invariant
            continue;
        }
        uint64_t thisPageVaddr = (INTRO_HYP_RODATA_START_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray3)[i*DIGEST_NUM_BYTES]), thisPageVaddr + 0x80000);
    }
    HashMeasure((uint8_t*)digestArray3, NUM_HYP_RODATA_PAGES * DIGEST_NUM_BYTES, temp_digest3);
    free(digestArray3);
    HashExtend(output_digest, temp_digest3);
    PrintDigest(temp_digest3);
    free(temp_digest3);
    numPagesMeasured += 1; // cheating since there are so few pages

    uint8_t (*temp_digest4)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray4)[NUM_TRAMP_ENTRY_TEXT_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_TRAMP_ENTRY_TEXT_PAGES, DIGEST_NUM_BYTES);
    printf("Measuring %d tramp text pages from %p to %p\n", NUM_TRAMP_ENTRY_TEXT_PAGES, INTRO_ENTRY_TRAMP_TEXT_START_VADDR, INTRO_ENTRY_TRAMP_TEXT_END_VADDR);
    for(int i=0; i<NUM_TRAMP_ENTRY_TEXT_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_ENTRY_TRAMP_TEXT_START_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray4)[i*DIGEST_NUM_BYTES]), thisPageVaddr + 0x80000);
    }
    HashMeasure((uint8_t*)digestArray4, NUM_TRAMP_ENTRY_TEXT_PAGES * DIGEST_NUM_BYTES, temp_digest4);
    free(digestArray4);
    HashExtend(output_digest, temp_digest4);
    PrintDigest(temp_digest4);
    free(temp_digest4);
    numPagesMeasured += NUM_TRAMP_ENTRY_TEXT_PAGES;

    uint8_t (*temp_digest5)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint64_t num_hibernate_bytes = INTRO_HIBERNATE_EXIT_TEXT_END_VADDR - INTRO_HIBERNATE_EXIT_TEXT_START_VADDR;
    printf("Measuring %d hibernate_exit text bytes starting at %p\n", num_hibernate_bytes, INTRO_HIBERNATE_EXIT_TEXT_START_VADDR);
    uint64_t hibernateStartVaddr = INTRO_HIBERNATE_EXIT_TEXT_START_VADDR;
    MeasureKernelRodataBytes((char*)memory_device, temp_digest5, hibernateStartVaddr + 0x80000, num_hibernate_bytes);
    HashExtend(output_digest, temp_digest5);
    PrintDigest(temp_digest5);
    free(temp_digest5);
    numBytesMeasured += num_hibernate_bytes;

    uint8_t (*temp_digest6)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint64_t num_relocate_kernel_bytes = INTRO_RELOCATE_NEW_KERNEL_END_VADDR - INTRO_RELOCATE_NEW_KERNEL_START_VADDR;
    printf("Measuring %d relocate_kernel text bytes starting at %p\n", num_relocate_kernel_bytes, INTRO_RELOCATE_NEW_KERNEL_START_VADDR);
    uint64_t relocateKernelVaddr = (INTRO_RELOCATE_NEW_KERNEL_START_VADDR);
    MeasureKernelRodataBytes((char*)memory_device, temp_digest6, relocateKernelVaddr + 0x80000, num_relocate_kernel_bytes);
    HashExtend(output_digest, temp_digest6);
    PrintDigest(temp_digest6);
    free(temp_digest6);
    numBytesMeasured += num_relocate_kernel_bytes;

    uint8_t (*temp_digest7)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("Measuring the whole page of the Identity Map page global directory starting at %p\n", INTRO_IDMAP_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest7, INTRO_IDMAP_PG_DIR_VADDR + 0x80000);
    HashExtend(output_digest, temp_digest7);
    PrintDigest(temp_digest7);
    free(temp_digest7);
    numPagesMeasured += 1;

    uint8_t (*temp_digest8)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("Measuring the whole page of the Trampoline page global directory starting at %p\n", INTRO_TRAMP_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest8, INTRO_TRAMP_PG_DIR_VADDR + 0x80000);
    HashExtend(output_digest, temp_digest8);
    PrintDigest(temp_digest8);
    free(temp_digest8);
    numPagesMeasured += 1;

    uint8_t (*temp_digest9)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("Measuring the whole page of the Reserved page global directory starting at %p\n", INTRO_RESERVED_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest9, INTRO_RESERVED_PG_DIR_VADDR + 0x80000);
    HashExtend(output_digest, temp_digest9);
    PrintDigest(temp_digest9);
    free(temp_digest9);
    numPagesMeasured += 1;

    uint8_t (*temp_digestA)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("Measuring the whole page of the Swapper page global directory starting at %p\n", INTRO_SWAPPER_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digestA, INTRO_SWAPPER_PG_DIR_VADDR + 0x80000);
    HashExtend(output_digest, temp_digestA);
    PrintDigest(temp_digestA);
    free(temp_digestA);
    numPagesMeasured += 1;

    uint8_t (*temp_digestB)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArrayB)[NUM_INIT_PGD_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_INIT_PGD_PAGES, DIGEST_NUM_BYTES);
    printf("Measuring %d Init page global directory pages from %p to %p\n", NUM_INIT_PGD_PAGES, INTRO_INIT_PG_DIR_VADDR, INTRO_END_VADDR);
    for(int i=0; i<NUM_INIT_PGD_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_INIT_PG_DIR_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArrayB)[i*DIGEST_NUM_BYTES]), thisPageVaddr + 0x80000);
    }
    HashMeasure((uint8_t*)digestArrayB, NUM_INIT_PGD_PAGES * DIGEST_NUM_BYTES, temp_digestB);
    free(digestArrayB);
    HashExtend(output_digest, temp_digestB);
    PrintDigest(temp_digestB);
    free(temp_digestB);
    numPagesMeasured += NUM_INIT_PGD_PAGES;

    uint8_t (*temp_digest2)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray2)[NUM_TEXT_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_TEXT_PAGES, DIGEST_NUM_BYTES);
    int numTextPagesActuallyMeasured = 0;
    for(int i=0; i<NUM_TEXT_PAGES; i++)
    {
        /* This is weird. The first and final 5% of the text segment is volatile. */
        /* In other words, we must not measure the 5% head or the 5% tail if we want a consistent measurement. */
        /* Specifically, even if we try to do a 4% head or a 4% tail, those measurements are not consistent. */
        /* Why is this? */

        /* I've narrowed it down somewhat. */
        /* From 3% to 3.4% is inconsistent. */
        /* From 3.4% to 3.8% is inconsistent. */
        /* From 3.8% to 4.2% is inconsistent. */
        /* From 4.6% to 5.0% is inconsistent. */

        /* It's actually just these several pages. */

        if(    i==0x05c 
            || i==0x05d
            || i==0x05f
            || i==0x060
            || i==0x06c
            || i==0x083
            || i==0x084
            || i==0x087
            || i==0x08a
            || i==0xb31
            || i==0xb33
            || i==0xb34
            || i==0xb36
            || i==0xb38
            || i==0xb39
            || i==0xb3b
            || i==0xb3f
          )
        {
            continue;
        }

        numTextPagesActuallyMeasured++;
        uint64_t thisPageVaddr = (INTRO_TEXT_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray2)[i*DIGEST_NUM_BYTES]), thisPageVaddr + 0x80000);
    }
    printf("Measured %d text pages between %p and %p\n", numTextPagesActuallyMeasured, INTRO_TEXT_VADDR, INTRO_ETEXT_VADDR);
    HashMeasure((uint8_t*)digestArray2, NUM_TEXT_PAGES * DIGEST_NUM_BYTES, temp_digest2);
    free(digestArray2);
    HashExtend(output_digest, temp_digest2);
    PrintDigest(temp_digest2);
    free(temp_digest2);
    numPagesMeasured += numTextPagesActuallyMeasured;

    printf("Measured %d pages in total\nMeasured %d bytes beyond that\n", numPagesMeasured, numBytesMeasured);

}

void MeasureKernelRodata(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    CollectRodataHashingAsWeGo(memory_device, output_digest);
}

