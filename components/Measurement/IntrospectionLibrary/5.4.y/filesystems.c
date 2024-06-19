/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * May 2024
 */

#include "xarray.c"

#define SUPER_BLOCK_S_TYPE 40
#define SUPER_BLOCK_S_ROOT 104
#define DENTRY_D_INODE 48
#define DENTRY_D_NAME 32
#define DENTRY_D_SUBDIRS 160
#define DENTRY_D_CHILD 144
#define INODE_I_DATA 376
#define ADDRESS_SPACE_I_PAGES 8

bool CanMeasureFileSystem()
{
    return true;
}


void MeasureFileInode(uint8_t* memory_device, uint64_t inode_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    // i_mapping refers to i_data
    uint64_t xarray_head_vaddr = ((uint64_t*)(memory_device+inode_paddr+INODE_I_DATA+ADDRESS_SPACE_I_PAGES+8))[0];
    if(xarray_head_vaddr == 0)
    {
        return;
    }
    uint64_t xarray_head_paddr = TranslationTableWalk(memdev, xarray_head_vaddr); // entry||node
    xa_dump(memdev, xarray_head_paddr, digest);
}
