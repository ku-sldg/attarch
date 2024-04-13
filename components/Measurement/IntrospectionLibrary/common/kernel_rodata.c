/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */


#include "../hash.h"


void MeasureKernelRodataBytes(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t startVaddr, uint64_t num_bytes)
{
    uint64_t pagePaddr = sysmap_virt_to_phys(startVaddr);
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

uint64_t RoundUpToNextPage(uint64_t x)
{
    return ((x + 0xFFF) >> 12) << 12;
}
uint64_t RoundDownToCurrentPage(uint64_t x)
{
    return (x >> 12) << 12;
}

void CollectRodataHashingAsWeGo(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    int numPagesMeasured = 0;
    int numPagesSkipped = 0;

    uint8_t (*temp_digest2)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray2)[NUM_TEXT_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_TEXT_PAGES, DIGEST_NUM_BYTES);
    int numTextPagesMeasured = 0;
    int numTextPagesSkipped = 0;
    for(int i=0; i<NUM_TEXT_PAGES; i++)
    {
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
            numTextPagesSkipped++;
            continue;
        }
        numTextPagesMeasured++;
        uint64_t thisPageVaddr = (INTRO_TEXT_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray2)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    printf("Measured %d text pages and skipped %d pages between %p and %p\n", numTextPagesMeasured, numTextPagesSkipped, INTRO_TEXT_VADDR, INTRO_ETEXT_VADDR);
    HashMeasure((uint8_t*)digestArray2, NUM_TEXT_PAGES * DIGEST_NUM_BYTES, temp_digest2);
    free(digestArray2);
    HashExtend(output_digest, temp_digest2);
    PrintDigest(temp_digest2);
    free(temp_digest2);
    numPagesMeasured += numTextPagesMeasured;
    numPagesSkipped += numTextPagesSkipped;

    uint8_t (*temp_digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray)[NUM_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_RODATA_PAGES, DIGEST_NUM_BYTES);
    int numRodataPagesMeasured = 0;
    int numRodataPagesSkipped = 0;
    for(int i=0; i<NUM_RODATA_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_START_RODATA_VADDR)+ i * INTRO_PAGE_SIZE;
        //printf("%04X\n", (thisPageVaddr & 0xFFFFFFF) >> 12);
        if(thisPageVaddr >= RoundDownToCurrentPage(INTRO_START_RO_AFTER_INIT_VADDR)
                && thisPageVaddr <= RoundUpToNextPage(INTRO_END_RO_AFTER_INIT_VADDR))
        {
            /* printf("Skipping ro_after_init data.\n"); */
            numRodataPagesSkipped++;
            continue;
        }
        numRodataPagesMeasured++;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    printf("\nMeasured %d rodata pages and skipped %d pages between %p and %p\n", numRodataPagesMeasured, numRodataPagesSkipped, INTRO_START_RODATA_VADDR, INTRO_END_RODATA_VADDR);
    HashMeasure((uint8_t*)digestArray, NUM_RODATA_PAGES * DIGEST_NUM_BYTES, temp_digest);
    free(digestArray);
    HashExtend(output_digest, temp_digest);
    PrintDigest(temp_digest);
    free(temp_digest);
    numPagesMeasured += numRodataPagesMeasured;
    numPagesSkipped += numRodataPagesSkipped;

    uint8_t (*temp_digest3)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray3)[NUM_HYP_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_HYP_RODATA_PAGES, DIGEST_NUM_BYTES);
    //printf("Measuring %d hyp_rodata pages from %p to %p\n", NUM_HYP_RODATA_PAGES, INTRO_HYP_RODATA_START_VADDR, INTRO_HYP_RODATA_END_VADDR);
    printf("\nMeasuring 1 and skipping 3 hyp_rodata pages between %p to %p\n", INTRO_HYP_RODATA_START_VADDR, INTRO_HYP_RODATA_END_VADDR);
    for(int i=0; i<NUM_HYP_RODATA_PAGES; i++)
    {
        if(i==0 || i==2 || i==3)
        {
            // these pages are certainly not invariant
            numPagesSkipped++;
            continue;
        }
        numPagesMeasured++;
        uint64_t thisPageVaddr = (INTRO_HYP_RODATA_START_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray3)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    HashMeasure((uint8_t*)digestArray3, NUM_HYP_RODATA_PAGES * DIGEST_NUM_BYTES, temp_digest3);
    free(digestArray3);
    HashExtend(output_digest, temp_digest3);
    PrintDigest(temp_digest3);
    free(temp_digest3);

    uint8_t (*temp_digest4)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArray4)[NUM_TRAMP_ENTRY_TEXT_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_TRAMP_ENTRY_TEXT_PAGES, DIGEST_NUM_BYTES);
    printf("\nMeasuring %d tramp text pages from %p to %p\n", NUM_TRAMP_ENTRY_TEXT_PAGES, INTRO_ENTRY_TRAMP_TEXT_START_VADDR, INTRO_ENTRY_TRAMP_TEXT_END_VADDR);
    for(int i=0; i<NUM_TRAMP_ENTRY_TEXT_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_ENTRY_TRAMP_TEXT_START_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray4)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    HashMeasure((uint8_t*)digestArray4, NUM_TRAMP_ENTRY_TEXT_PAGES * DIGEST_NUM_BYTES, temp_digest4);
    free(digestArray4);
    HashExtend(output_digest, temp_digest4);
    PrintDigest(temp_digest4);
    free(temp_digest4);
    numPagesMeasured += NUM_TRAMP_ENTRY_TEXT_PAGES;


    uint8_t (*temp_digest5)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint64_t num_hibernate_bytes = INTRO_HIBERNATE_EXIT_TEXT_END_VADDR - INTRO_HIBERNATE_EXIT_TEXT_START_VADDR;
    uint64_t num_relocate_kernel_bytes = INTRO_RELOCATE_NEW_KERNEL_END_VADDR - INTRO_RELOCATE_NEW_KERNEL_START_VADDR;
    printf("\nDiscovered %d hibernate_exit text bytes starting at %p\n", num_hibernate_bytes, INTRO_HIBERNATE_EXIT_TEXT_START_VADDR);
    printf("Discovered %d relocate_kernel text bytes starting at %p\n", num_relocate_kernel_bytes, INTRO_RELOCATE_NEW_KERNEL_START_VADDR);
    printf("Attempting to measure hibernate_exit text together with the relocate_new_kernel text as a single page.\n");
    uint64_t hibernateStartVaddr = INTRO_HIBERNATE_EXIT_TEXT_START_VADDR;
    uint64_t hibernateRelocatePageVaddr = RoundDownToCurrentPage(hibernateStartVaddr);
    MeasureKernelRodataPage((char*)memory_device, temp_digest5, hibernateStartVaddr);
    HashExtend(output_digest, temp_digest5);
    PrintDigest(temp_digest5);
    free(temp_digest5);
    numPagesMeasured++;

    /* uint8_t (*temp_digest5)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES); */
    /* uint64_t num_hibernate_bytes = INTRO_HIBERNATE_EXIT_TEXT_END_VADDR - INTRO_HIBERNATE_EXIT_TEXT_START_VADDR; */
    /* printf("Measuring %d hibernate_exit text bytes starting at %p\n", num_hibernate_bytes, INTRO_HIBERNATE_EXIT_TEXT_START_VADDR); */
    /* uint64_t hibernateStartVaddr = INTRO_HIBERNATE_EXIT_TEXT_START_VADDR; */
    /* MeasureKernelRodataBytes((char*)memory_device, temp_digest5, hibernateStartVaddr, num_hibernate_bytes); */
    /* HashExtend(output_digest, temp_digest5); */
    /* PrintDigest(temp_digest5); */
    /* free(temp_digest5); */
    /* numBytesMeasured += num_hibernate_bytes; */

    /* uint8_t (*temp_digest6)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES); */
    /* uint64_t num_relocate_kernel_bytes = INTRO_RELOCATE_NEW_KERNEL_END_VADDR - INTRO_RELOCATE_NEW_KERNEL_START_VADDR; */
    /* printf("Measuring %d relocate_kernel text bytes starting at %p\n", num_relocate_kernel_bytes, INTRO_RELOCATE_NEW_KERNEL_START_VADDR); */
    /* uint64_t relocateKernelVaddr = (INTRO_RELOCATE_NEW_KERNEL_START_VADDR); */
    /* MeasureKernelRodataBytes((char*)memory_device, temp_digest6, relocateKernelVaddr, num_relocate_kernel_bytes); */
    /* HashExtend(output_digest, temp_digest6); */
    /* PrintDigest(temp_digest6); */
    /* free(temp_digest6); */
    /* numBytesMeasured += num_relocate_kernel_bytes; */

    uint8_t (*temp_digest7)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("\nMeasuring the whole page of the Identity Map page global directory starting at %p\n", INTRO_IDMAP_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest7, INTRO_IDMAP_PG_DIR_VADDR);
    HashExtend(output_digest, temp_digest7);
    PrintDigest(temp_digest7);
    free(temp_digest7);
    numPagesMeasured += 1;

    uint8_t (*temp_digest8)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("\nMeasuring the whole page of the Trampoline page global directory starting at %p\n", INTRO_TRAMP_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest8, INTRO_TRAMP_PG_DIR_VADDR);
    HashExtend(output_digest, temp_digest8);
    PrintDigest(temp_digest8);
    free(temp_digest8);
    numPagesMeasured += 1;

    uint8_t (*temp_digest9)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("\nMeasuring the whole page of the Reserved page global directory starting at %p\n", INTRO_RESERVED_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digest9, INTRO_RESERVED_PG_DIR_VADDR);
    HashExtend(output_digest, temp_digest9);
    PrintDigest(temp_digest9);
    free(temp_digest9);
    numPagesMeasured += 1;

    uint8_t (*temp_digestA)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    printf("\nMeasuring the whole page of the Swapper page global directory starting at %p\n", INTRO_SWAPPER_PG_DIR_VADDR);
    MeasureKernelRodataPage((char*)memory_device, temp_digestA, INTRO_SWAPPER_PG_DIR_VADDR);
    HashExtend(output_digest, temp_digestA);
    PrintDigest(temp_digestA);
    free(temp_digestA);
    numPagesMeasured += 1;


    int numInitPages = (INTRO_INIT_END_VADDR - INTRO_INIT_BEGIN_VADDR) / INTRO_PAGE_SIZE;
    printf("\nSkipping %d Init pages from %p to %p\n", numInitPages, INTRO_INIT_BEGIN_VADDR, INTRO_INIT_END_VADDR);
    numPagesSkipped += numInitPages;
    int numDataPages = (RoundUpToNextPage(INTRO_EDATA_VADDR) - INTRO_DATA_VADDR) / INTRO_PAGE_SIZE;
    printf("Skipping %d Data pages from %p to %p\n", numDataPages, INTRO_DATA_VADDR, INTRO_EDATA_VADDR);
    numPagesSkipped += numDataPages; 
    int numBssPages = (RoundUpToNextPage(INTRO_BSS_STOP_VADDR) - INTRO_BSS_START_VADDR) / INTRO_PAGE_SIZE;
    printf("Skipping %d BSS pages from %p to %p\n", numBssPages, INTRO_BSS_START_VADDR, INTRO_BSS_STOP_VADDR);
    numPagesSkipped += numBssPages; 

    uint8_t (*temp_digestB)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    uint8_t (*digestArrayB)[NUM_INIT_PGD_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_INIT_PGD_PAGES, DIGEST_NUM_BYTES);
    printf("Measuring %d Init page global directory pages from %p to %p\n", NUM_INIT_PGD_PAGES, INTRO_INIT_PG_DIR_VADDR, INTRO_END_VADDR);
    for(int i=0; i<NUM_INIT_PGD_PAGES; i++)
    {
        uint64_t thisPageVaddr = (INTRO_INIT_PG_DIR_VADDR)+ i * INTRO_PAGE_SIZE;
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArrayB)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    HashMeasure((uint8_t*)digestArrayB, NUM_INIT_PGD_PAGES * DIGEST_NUM_BYTES, temp_digestB);
    free(digestArrayB);
    HashExtend(output_digest, temp_digestB);
    PrintDigest(temp_digestB);
    free(temp_digestB);
    numPagesMeasured += NUM_INIT_PGD_PAGES;


    //printf("Measured %d pages in total\nMeasured %d bytes beyond that\n", numPagesMeasured, numBytesMeasured);
    printf("\nMeasured %d pages in total\n", numPagesMeasured);
    printf("Skipped %d pages in total\n", numPagesSkipped);

    int numPagesShouldBeMeasured = (INTRO_END_VADDR - INTRO_TEXT_VADDR) / INTRO_PAGE_SIZE;
    int numPagesMissing = numPagesShouldBeMeasured - numPagesMeasured - numPagesSkipped;
    if(numPagesMissing > 0)
    {
        printf("It seems like %d page(s) from your System.map is not being taken into account.\n", numPagesMissing);
    }
    else
    {
        printf("All pages in your System.map were taken into account.\n");
    }

}

void MeasureKernelRodata(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    CollectRodataHashingAsWeGo(memory_device, output_digest);
}

