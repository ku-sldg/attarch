/*
 * Filesystem  Interpretation and Measurement
 * Michael Neises
 * May 2024
 */

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

bool InterpretType(uint8_t* memory_device, uint64_t sb_paddr)
{
    if(sb_paddr > 0xFFFF000000000000 + RAM_SIZE)
    {
        return false;
    }
    uint64_t type_vaddr = ((uint64_t*)(memory_device+sb_paddr+SUPER_BLOCK_S_TYPE))[0];
    uint64_t type_paddr = TranslateVaddr(memory_device, type_vaddr);
    if(type_paddr > 0xFFFF000000000000 + RAM_SIZE)
    {
        return false;
    }
    uint64_t name_vaddr = ((uint64_t*)(memory_device+type_paddr))[0];
    uint64_t name_paddr = TranslationTableWalk(memory_device, name_vaddr);
    if(name_paddr > 0xFFFF000000000000 + RAM_SIZE)
    {
        printf("bad name_paddr: %llx\n", name_paddr);
        return false;
    }
    printf("File System Type: %s\n", memory_device+name_paddr);
    return true;
}

void TraverseDentry(uint8_t* memory_device, uint64_t dentry_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    uint64_t name_vaddr = ((uint64_t*)(memory_device+dentry_paddr+DENTRY_D_NAME+8))[0];
    if(name_vaddr < 0xffff000000000000)
    {
        return;
    }
    uint64_t name_paddr = TranslationTableWalk(memory_device, name_vaddr);
    printf("(%s, ", memory_device+name_paddr);
    uint64_t inode_vaddr = ((uint64_t*)(memory_device+dentry_paddr+DENTRY_D_INODE))[0];
    uint64_t inode_paddr = TranslationTableWalk(memory_device, inode_vaddr);
    uint32_t imode = ((uint32_t*)(memory_device+inode_paddr))[0];
    if(S_ISDIR(imode))
    {
        printf("directory) ");
        uint64_t next_subdir_vaddr = ((uint64_t*)(memory_device+dentry_paddr+DENTRY_D_SUBDIRS))[0];
        if(next_subdir_vaddr == 0)
        {
            return;
        }
        uint64_t next_subdir_paddr = TranslationTableWalk(memory_device, next_subdir_vaddr);
        uint64_t subdir_iter = next_subdir_paddr;
        do
        {
            TraverseDentry(memory_device, subdir_iter - DENTRY_D_CHILD, digest);
            uint64_t temp_vaddr = ((uint64_t*)(memory_device+subdir_iter))[0];
            subdir_iter = TranslationTableWalk(memory_device, temp_vaddr);
        } while(subdir_iter != next_subdir_paddr);
        /* printf("\n"); */

    }
    else if(S_ISREG(imode))
    {
        printf("file, ");
        if(strcmp(memory_device+name_paddr, "useram")==0)
        {
            /* printf("take action on a specific file\n"); */
        }
        MeasureFileInode(memory_device, inode_paddr, digest);
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
        printf("?) ");
    }

}

void InterpretSRoot(uint8_t* memory_device, uint64_t sb_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    uint64_t sroot_vaddr = ((uint64_t*)(memory_device+sb_paddr))[SUPER_BLOCK_S_ROOT/8];
    uint64_t sroot_paddr = TranslationTableWalk(memory_device, sroot_vaddr);
    TraverseDentry(memory_device, sroot_paddr, digest);
}

void ActOnSuperblock(uint8_t* memory_device, uint64_t sb_paddr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    if(InterpretType(memory_device, sb_paddr))
    {
        InterpretSRoot(memory_device, sb_paddr, digest);
    }
}

void MeasureFileSystems(uint8_t* memory_device, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    if(!CanMeasureFileSystem())
    {
        return;
    }
    // it is the administrative list_head (not within a struct super_block)
    uint64_t sbs_paddr = TranslateVaddr(memory_device, INTRO_SUPER_BLOCKS_VADDR);
    uint64_t sb_iter = sbs_paddr;
    sb_iter = TranslateVaddr(memory_device, ((uint64_t*)(memory_device+sb_iter))[0]);
    while(sb_iter != sbs_paddr)
    {
        ActOnSuperblock(memory_device, sb_iter, digest);
        uint64_t temp_vaddr = ((uint64_t*)(memory_device+sb_iter))[0];
        sb_iter = TranslateVaddr(memory_device, temp_vaddr);
    } 
    printf("Done measuring file systems\n");
}
