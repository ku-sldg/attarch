/* Introspection tasks pertaining to linux kernel system call table */
/* Michael Neises */
/* April 2024 */

#define SYS_CALL_TABLE_VADDR 0xffff800010a31778
#define SYS_CALL_TABLE_ENTRIES 435

void MeasureSystemCallTable(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    // Measure the system call table
    // The system call table has entries that are function pointers.
    // So the entries are of type uint64_t.
    // 435 entries, by inspection of syscall.tbl file
    // The output digest is consistent between boots
    // and across builds
    uint64_t sctPaddr = TranslateVaddr(memory_device, SYS_CALL_TABLE_VADDR);
    HashMeasure(memory_device+sctPaddr, sizeof(uint64_t) * SYS_CALL_TABLE_ENTRIES, output_digest);
}

