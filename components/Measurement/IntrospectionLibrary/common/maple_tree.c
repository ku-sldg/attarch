
#include "maple_tree_xarray_reqs.c"
#include "maple_tree_top_level_defs.c"
/* #include "maple_tree_validation.c" */
#include "maple_tree_dump.c"

void StartDumpProcedure(char* memory_device, uint64_t maple_tree_struct_paddr, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    struct maple_tree* mtp = ((struct maple_tree*)((char*)memory_device+maple_tree_struct_paddr));
    mt_dump(mtp, memory_device, pgd_paddr, rodataDigest);
}
