/* Introspection tasks pertaining to linux kernel system call table */
/* Michael Neises */
/* April 2024 */

void DigestSystemCallTable(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    // Measure the system call table
    // The system call table has entries that are function pointers.
    // So the entries are of type uint64_t.
    // 435 entries, by inspection of syscall.tbl file
    // The output digest is consistent between boots
    // and across builds
    uint64_t sctPaddr = TranslateVaddr(memory_device, INTRO_SYS_CALL_TABLE_VADDR);
    HashMeasure(memory_device, sctPaddr, sizeof(uint64_t) * NUM_SYS_CALL_TABLE_ENTRIES, output_digest);
}

EvidenceBundle* MeasureSystemCallTable(uint8_t* memory_device, int* count)
{
    const char name[56] = "SystemCallTable";
    uint8_t (*digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    DigestSystemCallTable(memory_device, digest);
    EvidenceBundle* bundle = AllocBundle(&RODATA_TYPE, name, digest);
    free(digest);
    *count = 1;
    return bundle;
}

