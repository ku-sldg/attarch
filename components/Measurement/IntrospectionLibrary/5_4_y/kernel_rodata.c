/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */


void MeasureKernelRodataPage(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = 0;
    if(pageVaddr < 0xFFFF800000000000)
    {

       /* This marks the virtual address as belonging to Linux 4.x.y */
       /* So we will translate the address in a special way. */
       /* In particular, we will simply subtract KIMAGE_VADDR, */
       /*    which is the offset of the kernel in physical memory. */

        pagePaddr = pageVaddr-INTRO_KIMAGE_VADDR; // this is simply vaddr - kimage_vaddr
    }
    else
    {

        /* Otherwise, we should handle this address in a different way. */
        /*     In particular, we will subject it to the normal virtual address translation procedures. */
        /* In Linux 5, the memory layout was changed so that kernel addresses were higher than PAGE_OFFSET. */
        /*     I believe it was the opposite previous to that, otherwise it must be the case that the kernel */
        /*     was translating its symbols in a way I've never read about. Perhaps the kernel addresses were always high, */
        /*     except in the special case of the System.map addresses, which, despite being kernel addresses, */
        /*     are for some reason stored as virtual addresses in the user range. */

        pagePaddr = TranslateVaddr(memory_device, pageVaddr);
    }
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}

void CollectRodataHashingAsWeGo(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{

    uint64_t RoundUpToNextPage(uint64_t x)
    {
        return ((x + 0xFFF) >> 12) << 12;
    }

    uint64_t RoundDownToCurrentPage(uint64_t x)
    {
        return (x >> 12) << 12;
    }

    uint8_t (*digestArray)[NUM_RODATA_PAGES * DIGEST_NUM_BYTES] = calloc(NUM_RODATA_PAGES, DIGEST_NUM_BYTES);
    /* We found some "ro_after_init" data */
    /* This offers us a unique chance to do provisioning for this data at boot-time */
    /* There is simply no other way to perform a meaningful appraisal of this data, */
    /* except perhaps in the case where you are able to somehow predict the data...? */
    for(int i=0; i<NUM_RODATA_PAGES; i++)
    {
        uint64_t thisPageVaddr = INTRO_START_RODATA_VADDR + i * INTRO_PAGE_SIZE;
        if(thisPageVaddr >= RoundDownToCurrentPage(INTRO_START_RO_AFTER_INIT_VADDR)
                && thisPageVaddr <= RoundUpToNextPage(INTRO_END_RO_AFTER_INIT_VADDR))
        {
            /* printf("Skipping ro_after_init data.\n"); */
            continue;
        }
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisPageVaddr);
    }
    HashMeasure((uint8_t*)digestArray, NUM_RODATA_PAGES * DIGEST_NUM_BYTES, output_digest);
    free(digestArray);
}

void MeasureKernelRodata(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    CollectRodataHashingAsWeGo(memory_device, output_digest);
}

