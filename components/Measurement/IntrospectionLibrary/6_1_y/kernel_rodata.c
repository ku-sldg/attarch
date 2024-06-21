void MeasureKernelRodataPage(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = TranslateVaddr(memory_device, pageVaddr);
    MAYBE_EXIT(pagePaddr);
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}
