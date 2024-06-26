/* Introspection tasks pertaining to linux kernel (version 4.9.307) rodata */
/* Michael Neises */
/* 19 August 2022 */

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
        MeasureKernelRodataPage((char*)memory_device, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisPageVaddr + SWAPPER_OFFSET);
    }
    HashMeasure((uint8_t*)digestArray, 0, NUM_RODATA_PAGES * DIGEST_NUM_BYTES, output_digest);
    free(digestArray);
}

EvidenceBundle* MeasureKernelRodata(uint8_t* memory_device, int* count)
{
    const char name[56] = "KernelRodata";
    uint8_t (*digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    CollectRodataHashingAsWeGo(memory_device, digest);
    EvidenceBundle* bundle = AllocBundle(&RODATA_TYPE, name, digest);
    free(digest);
    *count = 1;
    return bundle;
}

