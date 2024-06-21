
#define MM_OFFSET 1096
#define PID_OFFSET 1272
#define PARENT_OFFSET 1296
#define CHILDREN_OFFSET 1304
#define SIBLING_OFFSET 1320
#define REAL_CRED_OFFSET 1688
#define COMM_OFFSET 1712
#define PGD_OFFSET 64

bool CanMeasureTasks()
{
    return false;
}

/* This function takes an address "vma" as input.
** vma should point to a genuine Virtual Memory Area struct in VM memory.
** The argument pgdPaddr is the address to the related Page Global Directory.
** The remainder of the arguments are pointers which are loaded with relevant
** output data
*/
void InterpVMA(uint8_t* memory_device, uint64_t vma, uint64_t* start, uint64_t* size, uint64_t* next, uint64_t* flags, uint64_t pgdPaddr)
{
    uint64_t vm_start = ((uint64_t*)((char*)memory_device+vma))[0];
    /* TODO */
    /* We need this conditional, but we're not entirely sure why */ 
    /* Actually I think this is the effective size of the process address space, */
    /* which doesn't really make sense because the address space should theoretically */
    /* be enormous on a 64-bit system, */
    /* but on the other hand, we only seem to enter this first condition on */
    /* weird errors. Like once we enqueued a task twice to be measured, */
    /* and on the second measurement, all the addresses were bogus and we hit */
    /* this condition several times fruitlessly before moving on */
    if((unsigned long)0xF0000000 < (unsigned long)vm_start)
    {
        /* printf("calling intro_virt_to_phys on %p\n", vm_start); */
        *start = intro_virt_to_phys(vm_start);
        /* printf("after: %p\n", *start); */
    }
    else
    {
        *start = TranslationTableWalkSuppliedPGD(memory_device, vm_start, pgdPaddr);
    }

    /* Here we retrieve data from the vma struct we just found */
    *size = ((uint64_t*)((char*)memory_device+vma))[1] - ((uint64_t*)((char*)memory_device+vma))[0];
    uint64_t nextVaddr = ((uint64_t*)((char*)memory_device+vma))[2];
    uint64_t prevVaddr = ((uint64_t*)((char*)memory_device+vma))[3];
    *flags = ((uint64_t*)((char*)memory_device+vma))[10];
    if(nextVaddr == 0)
    {
        *next = 0;
        return;
    }
    // we intend for this to ensure impossible address translations do not take
    // place...
    // TODO
    // This is another conditional we're not perfectly sure about
    if((long)nextVaddr < (long)0xFFFF80000000)
    {
        *next = TranslateVaddr(memory_device, nextVaddr);
    }
    else
    {
        printf("walk supplied pgd\n");
        *next = TranslationTableWalkSuppliedPGD(memory_device, nextVaddr, pgdPaddr);
    }
}

/* CrawlVMAs is used to traverse the list of VMAs belonging to a task.
** It takes an address "vma" which points to a genuine vma_struct in VM memory.
** It takes an address "pgdPaddr" to the relevant Page Global Directory.
** It outputs a hash digest into eponymous pointer argument.
*/
void CrawlVMAs(uint8_t* memory_device, uint64_t vma, uint64_t pgdPaddr, uint8_t (*rodataDigest)[DIGEST_NUM_BYTES])
{
    uint64_t start;
    uint64_t size;
    uint64_t next;
    uint64_t flags;
    DebugLog("before interpVMA\n");
    InterpVMA(memory_device, vma, &start, &size, &next, &flags, pgdPaddr);
    DebugLog("after\n");
    if( start + size < RAM_SIZE
            && ((char*)memory_device+start)[0] == 0x7f
            && ((char*)memory_device+start)[1] == 'E' 
            && ((char*)memory_device+start)[2] == 'L' 
            && ((char*)memory_device+start)[3] == 'F' )
    {
        DebugLog("before measure elf rodata\n");
        if(TryMeasureElfRodata(memory_device, start, pgdPaddr, rodataDigest))
        {
            DebugLog("after\n");
            // we got a good digest
            // we expect a task to have at most one ELF header in memory, so we
            // can return right away.
            return;
        }
        DebugLog("after\n");
    }
    if(next != 0)
    {
        CrawlVMAs(memory_device, next, pgdPaddr, rodataDigest);
    }
}

/* InterpretMemory takes a pointer to a valid task_struct in VM memory "task"
** and returns a hash digest of its rodata (if it exists)
*/
void InterpretMemory(uint8_t* memory_device, uint64_t task, uint8_t (*rodataDigest)[DIGEST_NUM_BYTES])
{
    uint64_t mmAddrLoc = task + 1024;
    uint64_t mmAddr = ((uint64_t*)((char*)memory_device+mmAddrLoc))[0];
    if(mmAddr==0)
    {
        mmAddr = ((uint64_t*)((char*)memory_device+mmAddrLoc+8))[0];
        if(mmAddr==0)
        {
            return;
        }
    }
    uint64_t mm = TranslateVaddr(memory_device, mmAddr);
    uint64_t pgdVaddr = ((uint64_t*)((char*)memory_device+mm+64))[0];
    uint64_t pgdPaddr = TranslateVaddr(memory_device, pgdVaddr);
    uint64_t mmapAddr = ((uint64_t*)((char*)memory_device+mm))[0];
    uint64_t mmap = TranslateVaddr(memory_device, mmapAddr);
    // these linked lists are apparently null terminated on the ends,
    // so here we put ourselves at the "leftmost" position,
    // before we crawl strictly to the right
    uint64_t prevVaddr = ((uint64_t*)((char*)memory_device+mmap))[3];
    while(prevVaddr != 0)
    {
        mmap = TranslateVaddr(memory_device, prevVaddr);
        prevVaddr = ((uint64_t*)((char*)memory_device+mmap))[3];
    }
    /* One ELF per file */
    DebugLog("before crawlVMAs\n");
    CrawlVMAs(memory_device, mmap, pgdPaddr, rodataDigest);
    DebugLog("after\n");
}

