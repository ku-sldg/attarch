/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * June 2024
 * Measurement Stub
 */

#define SUPER_BLOCK_S_TYPE 0
#define SUPER_BLOCK_S_ROOT 0
#define DENTRY_D_INODE 0
#define DENTRY_D_NAME 0
#define DENTRY_D_SUBDIRS 0
#define DENTRY_D_CHILD 0
#define INODE_I_DATA 0
#define ADDRESS_SPACE_I_PAGES 0

bool CanMeasureFileSystem()
{
    return false;
}

void MeasureFileInode(uint8_t* memory_device, uint64_t inode_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    return;
}
