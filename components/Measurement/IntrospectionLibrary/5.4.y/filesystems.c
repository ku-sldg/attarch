/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * May 2024
 */

#include "xarray.c"

#define S_IFMT  00170000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)

#define S_LIST 0
#define S_TYPE 40
#define S_ROOT 104
#define DENTRY_INODE 48
#define DENTRY_DNAME 32
#define DENTRY_DPARENT 24
#define DENTRY_SUBDIRS 160
#define DENTRY_CHILD 144
#define INODE_INO 64
#define INODE_MODE 0
#define FST_NAME 0

#define AS_PAGES 8
#define I_DATA 376
#define XA_HEAD 8
#define AS_NRPAGES 88
#define I_FOP 360
#define FO_READ 16

void PrintListHead(uint8_t* memdev, uint64_t paddr)
{
    printf("List head:\n\t");
    for(int i=0; i<16; i++)
    {
        if(i>0 && i%8==0)
        {
            printf("\n\t");
        }
        printf("%02X", (memdev + paddr)[i]);
    }
    printf("\n");
}

void InterpretType(uint8_t* memdev, uint64_t sb_paddr)
{
    uint64_t type_vaddr = ((uint64_t*)(memdev+sb_paddr))[S_TYPE/8];
    uint64_t type_paddr = TranslationTableWalk(memdev, type_vaddr);
    uint64_t name_vaddr = ((uint64_t*)(memdev+type_paddr))[0];
    uint64_t name_paddr = TranslationTableWalk(memdev, name_vaddr);
    printf("File System Type: %s\n", memdev+name_paddr);
}

void TraverseDentry(uint8_t* memdev, uint64_t dentry_paddr)
{
    uint64_t name_vaddr = ((uint64_t*)(memdev+dentry_paddr+DENTRY_DNAME+8))[0];
    if(name_vaddr < 0xffff000000000000)
    {
        /* printf("bad.\n"); */
        return;
    }
    uint64_t name_paddr = TranslationTableWalk(memdev, name_vaddr);
    printf("(%s, ", memdev+name_paddr);

    uint64_t inode_vaddr = ((uint64_t*)(memdev+dentry_paddr+DENTRY_INODE))[0];
    uint64_t inode_paddr = TranslationTableWalk(memdev, inode_vaddr);

    uint32_t imode = ((uint32_t*)(memdev+inode_paddr))[0];

    if(S_ISDIR(imode))
    {
        printf("directory) ");
        uint64_t next_subdir_vaddr = ((uint64_t*)(memdev+dentry_paddr+DENTRY_SUBDIRS))[0];
        if(next_subdir_vaddr == 0)
        {
            return;
        }
        uint64_t next_subdir_paddr = TranslationTableWalk(memdev, next_subdir_vaddr);
        uint64_t subdir_iter = next_subdir_paddr;
        do
        {
            TraverseDentry(memdev, subdir_iter - DENTRY_CHILD);
            uint64_t temp_vaddr = ((uint64_t*)(memdev+subdir_iter))[0];
            subdir_iter = TranslationTableWalk(memdev, temp_vaddr);
        } while(subdir_iter != next_subdir_paddr);
        printf("\n");

    }
    else if(S_ISREG(imode))
    {
        printf("file) ");
        if(strcmp(memdev+name_paddr, "useram")==0)
        /* if(strcmp(memdev+name_paddr, "manifest.json")==0) */
        {
            printf("\n\ntake action on useram file\n\n");

            uint64_t xarray_head_vaddr = ((uint64_t*)(memdev+inode_paddr+I_DATA+AS_PAGES+8))[0];
            if(xarray_head_vaddr == 0)
            {
                printf("empty xarray!\n");
                return;
            }
            printf("here %llx\n", xarray_head_vaddr);
            uint64_t xarray_head_paddr = TranslationTableWalk(memdev, xarray_head_vaddr); // entry||node

            xa_dump(memdev, xarray_head_paddr);

        }
    }
    else if(S_ISLNK(imode))
    {
        printf("symlink) ");
    }
    else if(S_ISCHR(imode))
    {
        printf("chr) ");
    }
    else if(S_ISBLK(imode))
    {
        printf("block) ");
    }
    else if(S_ISFIFO(imode))
    {
        printf("fifo) ");
    }
    else
    {
        printf("???) ");
    }

}

void InterpretSRoot(uint8_t* memdev, uint64_t sb_paddr)
{
    uint64_t sroot_vaddr = ((uint64_t*)(memdev+sb_paddr))[S_ROOT/8];
    uint64_t sroot_paddr = TranslationTableWalk(memdev, sroot_vaddr);
    TraverseDentry(memdev, sroot_paddr);
}

void ActOnSuperblock(uint8_t* memory_device, uint64_t sb_paddr)
{
    InterpretType(memory_device, sb_paddr);
    InterpretSRoot(memory_device, sb_paddr);
}

void MeasureFileSystems(uint8_t* memory_device)
{
    uint64_t sbs_vaddr = 0xffff8000111b4148;

    // sbs_paddr is the offset to the "super_blocks" symbol.
    // it is the administrative list_head (not within a struct super_block)
    uint64_t sbs_paddr = TranslateVaddr(memory_device, sbs_vaddr);
    uint64_t sb_iter = sbs_paddr;
    sb_iter = TranslationTableWalk(memory_device, ((uint64_t*)(memory_device+sb_iter))[0]);
    bool aye = false; // this bool makes output readable right now
                      // without it, every file system gets fully printed
                      // there are about a dozen, and some are densely packed
                      // But the second FS is the typical one,
                      // So let's only print two for now.
    while(sb_iter != sbs_paddr)
    {
        ActOnSuperblock(memory_device, sb_iter);
        uint64_t temp_vaddr = ((uint64_t*)(memory_device+sb_iter))[0];
        sb_iter = TranslationTableWalk(memory_device, temp_vaddr);
        if(aye)
        {
            return;
        }
        aye = true;
    } 
    printf("Done measuring file systems\n");
}
