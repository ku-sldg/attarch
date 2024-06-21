
bool CanMeasureTasks()
{
    return false;
}

void GetTaskNamePointer(uint8_t* memory_device, uint64_t task, char (*output_name)[TASK_COMM_LEN])
{
    char* index = ((char*)memory_device) + task + 1640;
    for(int i=0; i<TASK_COMM_LEN; i++)
    {
       (*output_name)[i] = (uint8_t)(index[i]);
    }
}

/* We use this function to check whether a given address "task"
** acutally points to a genuine task_struct in VM memory.
*/
bool ValidateTaskStruct(uint8_t* memory_device, uint64_t task)
{
    if(task > RAM_SIZE)
    {
        /* printf("out of range\n"); */
        return false;
    }
    int nameLoc = task + 1640;
    if(((char*)memory_device+nameLoc)[0] == '\0')
    {
        /* printf("null name\n"); */
        return false;
    }
    for(int i=0; i<TASK_COMM_LEN; i++)
    {
        if(((char*)memory_device+nameLoc)[i] > 127)
        {
            /* printf("illegal characters in name\n"); */
            /* int thisShouldBeNameData = nameLoc; */
            /* introspectScan(&thisShouldBeNameData, 64, "offending task?:\n"); */
            return false;
        }
    }
    return true;
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

/* takes an address to a task_struct in vm memory
** returns its pid
*/
void GetPID(uint8_t* memory_device, uint64_t task, int* myPid)
{
    int myPidLoc = task + 1200;
    *myPid =((int*)((char*)memory_device+myPidLoc))[0];
}

/* takes an address to a task_struct in vm memory
** returns its pid and its parent's pid
*/
void GetPIDs(uint8_t* memory_device, uint64_t task, int* myPid, int* parentPid)
{
    GetPID(memory_device, task, myPid);
    uint64_t parentAddrLoc = task + 1216;
    uint64_t parentAddr = ((uint64_t*)((char*)memory_device+parentAddrLoc))[0];
    uint64_t parent = TranslateVaddr(memory_device, parentAddr);
    if(ValidateTaskStruct(memory_device, parent))
    {
        GetPID(memory_device, parent, parentPid);
    }
}

/* takes an address to a task_struct in vm memory
** returns a cred struct populated with the task's creds
*/
void InterpretCred(uint8_t* memory_device, uint64_t task, struct cred* cred)
{
    uint64_t credAddrLoc = task + 1640 - 8 - 8; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memory_device+credAddrLoc))[0];
    uint64_t credPaddr = TranslateVaddr(memory_device, credVaddr);

    cred->usage = ((int*)((char*)memory_device+credPaddr))[0];
    cred->uid = ((int*)((char*)memory_device+credPaddr+4))[0];
    cred->gid = ((int*)((char*)memory_device+credPaddr+8))[0];
    cred->suid = ((int*)((char*)memory_device+credPaddr+12))[0];
    cred->sgid = ((int*)((char*)memory_device+credPaddr+16))[0];
    cred->euid = ((int*)((char*)memory_device+credPaddr+20))[0];
    cred->egid = ((int*)((char*)memory_device+credPaddr+24))[0];
    cred->fsuid = ((int*)((char*)memory_device+credPaddr+28))[0];
    cred->fsgid = ((int*)((char*)memory_device+credPaddr+32))[0];
    cred->securebits = ((int*)((char*)memory_device+credPaddr+36))[0];
    cred->cap_inheritable = ((uint64_t*)((char*)memory_device+credPaddr+40))[0];
    cred->cap_permitted = ((uint64_t*)((char*)memory_device+credPaddr+48))[0];
    cred->cap_effective = ((uint64_t*)((char*)memory_device+credPaddr+56))[0];
    cred->cap_bset = ((uint64_t*)((char*)memory_device+credPaddr+64))[0];
    cred->cap_ambient = ((uint64_t*)((char*)memory_device+credPaddr+72))[0];
    cred->jit_keyring = ((char*)memory_device+credPaddr+80)[0];
    cred->session_keyring = ((uint64_t*)((char*)memory_device+credPaddr+88))[0];
    cred->process_keyring = ((uint64_t*)((char*)memory_device+credPaddr+96))[0];
    cred->thread_keyring = ((uint64_t*)((char*)memory_device+credPaddr+104))[0];
    cred->request_key_auth = ((uint64_t*)((char*)memory_device+credPaddr+112))[0];
    cred->security = ((uint64_t*)((char*)memory_device+credPaddr+120))[0];
    cred->user_struct = ((uint64_t*)((char*)memory_device+credPaddr+128))[0];
    cred->user_namespace = ((uint64_t*)((char*)memory_device+credPaddr+136))[0];
    cred->group_info = ((uint64_t*)((char*)memory_device+credPaddr+144))[0];
}

/* InterpretTaskStruct takes an address to a task_struct in vm memory
** it returns the addresses of it's leftmost child and left sibling
*/
void InterpretTaskStruct(uint8_t* memory_device, uint64_t thisTaskStructPaddr, uint64_t* children, uint64_t* sibling)//, uint64_t* parent)
{
    int childLoc = thisTaskStructPaddr + 1232;
    uint64_t firstChild = ((uint64_t*)((char*)memory_device+childLoc))[0];
    /* printf("firstChild: %p\n", firstChild); */
    *children = TranslateVaddr(memory_device, firstChild)-1248;

    int siblingLoc = thisTaskStructPaddr + 1248;
    uint64_t leftSibling = ((uint64_t*)((char*)memory_device+siblingLoc))[0];
    /* printf("leftSibling: %p\n", leftSibling); */
    *sibling = TranslateVaddr(memory_device, leftSibling)-1248;
}

