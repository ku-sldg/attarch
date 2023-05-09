/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */


#include "../hash.h"

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
    uint8_t (*digestArray)[NUM_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_RODATA_PAGES, DIGEST_NUM_BYTES);
    /* We found some "ro_after_init" data */
    /* This offers us a unique chance to do provisioning for this data at boot-time */
    /* There is simply no other way to perform a meaningful appraisal of this data, */
    /* except perhaps in the case where you are able to somehow predict the data...? */
    for(int i=0; i<NUM_RODATA_PAGES; i++)
    {
        uint64_t thisPageVaddr = START_RODATA + i * INTRO_PAGE_SIZE;
        if(thisPageVaddr == RoundDownToCurrentPage(START_READ_ONLY_AFTER_INIT)
                || thisPageVaddr == RoundUpToNextPage(END_READ_ONLY_AFTER_INIT))
        {
            /* printf("this was a ro_after_init page...\n"); */
            continue;
        }
        MeasureKernelPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    HashMeasure((uint8_t*)digestArray, NUM_RODATA_PAGES * DIGEST_NUM_BYTES, output_digest);
    free(digestArray);
}

void MeasureKernelRodata(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    CollectRodataHashingAsWeGo(memory_device, output_digest);
}

