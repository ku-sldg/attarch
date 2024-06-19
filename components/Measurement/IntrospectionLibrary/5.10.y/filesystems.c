/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * June 2024
 * Measurement Stub
 */

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
    return false;
}


void MeasureFileInode(uint8_t* memory_device, uint64_t inode_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    return;
}
