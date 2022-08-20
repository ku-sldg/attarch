/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */


#include "../hash.h"

#define intro_page_size 0x1000

#define __start_rodata 0xffff0000087c0000
#define __end_rodata   0xffff000008bbf000

#define __start_data_ro_after_init 0xffff00000899d7dc
#define __end_data_ro_after_init   0xffff00000899e160

uint64_t RoundUpToNextPage(uint64_t x)
{
        return ((x + 0xFFF) >> 12) << 12;
}

uint64_t RoundDownToCurrentPage(uint64_t x)
{
        return (x >> 12) << 12;
}

void MeasureKernelRodataPage(uint8_t* digest, uint64_t pageVaddr)
{
    uint64_t pagePaddr = intro_virt_to_phys(pageVaddr-0x8000000);
    HashMeasure( ((char*)memdev+pagePaddr), 4096, digest );
}

void CollectRodataHashingAsWeGo(uint8_t* digest)
{
    int numRodataPages = (__end_rodata - __start_rodata)/0x1000;
    uint8_t* digestArray = calloc(numRodataPages, 64);
    /* We found some "ro_after_init" data */
    /* This offers us a unique chance to do provisioning for this data at boot-time */
    /* There is simply no other way to perform a meaningful appraisal of this data, */
    /* except perhaps in the case where you are able to somehow predict the data...? */
    for(int i=0; i<numRodataPages; i++)
    {
        uint64_t thisPageVaddr = __start_rodata + i * 0x1000;
        if(thisPageVaddr == RoundDownToCurrentPage(__start_data_ro_after_init)
        || thisPageVaddr == RoundUpToNextPage(__end_data_ro_after_init))
        {
            /* printf("this was a ro_after_init page...\n"); */
            continue;
        }
        MeasureKernelRodataPage(digestArray+i*64, thisPageVaddr); 
    }
    HashMeasure(digestArray, numRodataPages * 64, digest);
    free(digestArray);
}

void MeasureKernelRodata(uint8_t* digest)
{
    CollectRodataHashingAsWeGo(digest);
}
