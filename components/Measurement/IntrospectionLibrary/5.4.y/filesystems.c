/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * May 2024
 */

#include "xarray.c"

#define AS_PAGES 8
#define I_DATA 376

void MeasureFileInode(uint8_t* memory_device, uint64_t inode_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    // i_mapping refers to i_data
    uint64_t xarray_head_vaddr = ((uint64_t*)(memory_device+inode_paddr+I_DATA+AS_PAGES+8))[0];
    if(xarray_head_vaddr == 0)
    {
        return;
    }
    uint64_t xarray_head_paddr = TranslationTableWalk(memdev, xarray_head_vaddr); // entry||node
    xa_dump(memdev, xarray_head_paddr, digest);
}
