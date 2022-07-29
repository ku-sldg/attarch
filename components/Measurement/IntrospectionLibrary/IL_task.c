/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#include "IL_struct_interp.c"
#include "IL_elf_header.c"

struct cred {
    int usage;
    int uid, gid, suid, sgid, euid, egid, fsuid, fsgid;
    int securebits;
    uint64_t cap_inheritable, cap_permitted, cap_effective, cap_bset, cap_ambient;
    char jit_keyring;
    uint64_t session_keyring, process_keyring, thread_keyring, request_key_auth, security;
    uint64_t user_struct, user_namespace, group_info;
};

struct intro_mm_struct {
    uint64_t start_code;
    uint64_t end_code;
    uint64_t start_data;
    uint64_t end_data;
    uint64_t start_brk;
    uint64_t brk;
    uint64_t start_stack;
    uint64_t args_start;
    uint64_t args_end;
    uint64_t env_start;
    uint64_t env_end;
};

struct TaskMeasurement
{
    char name[16];
    int uid;
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
    struct cred cred;
    char rodataDigest[64];
};
void PrintTaskEvidence(struct TaskMeasurement* msmt)
{
    char myPid[10];
    sprintf(myPid, "%ld", msmt->myPid);
    char parentPid[10];
    sprintf(parentPid, "%ld", msmt->parentPid);
    char suid[10];
    sprintf(suid, "%d", msmt->cred.suid);
    introLog(9,
        "Task Evidence:\n\tName: ",
        msmt->name,
        "\n\tPID: ",
        &myPid,
        "\n\tParent PID: ",
        &parentPid,
        "\n\tSUID: ",
        &suid,
        "\n\tRead-only Data SHA512 Digest:\n");
    PrintDigest(msmt->rodataDigest);
}

void GetTaskName(uint64_t task, char* name)
{
    int nameLoc = task + 1640;
    for(int i=0; i<16; i++)
    {
        name[i] = ((char*)memdev+nameLoc)[i];
    }
}

bool ValidateTaskStruct(uint64_t task)
{
    if(task > 0x8001000)
    {
        //printf("out of range\n");
        return false;
    }
    int nameLoc = task + 1640;
    if(((char*)memdev+nameLoc)[0] == '\0')
    {
        //printf("null name\n");
        return false;
    }
    for(int i=0; i<16; i++)
    {
        if(((char*)memdev+nameLoc)[i] > 127)
        {
            //printf("illegal characters in name\n");
            return false;
        }
    }
    return true;
}

void InterpVMA(uint64_t vma, uint64_t* start, uint64_t* size, uint64_t* next, uint64_t* flags, uint64_t pgdPaddr, bool isKernelTask)
{
    /* printf("vm_start\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[0]); */
    /* printf("vm_end\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[1]); */
    /* printf("vm_next\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[2]); */
    /* printf("vm_prev\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[3]); */
    /* printf("vm_rb\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[4]); */
    /* printf("rb_subtree_gap\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[7]); */
    /* printf("vm_mm\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[8]); */
    /* printf("page_prot\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[9]); */
    /* printf("page_flags\t\t%p\n", ((uint64_t*)((char*)memdev+vma))[10]); */

    uint64_t vm_start = ((uint64_t*)((char*)memdev+vma))[0];
    /* printf("before interpvma translate\n"); */
    //TODO
    /* uint64_t test = isKernelTask ? intro_virt_to_phys(vm_start) : TranslationTableWalk(vm_start); */
    /* uint64_t test = GetPhysAddr(vm_start); */
    /* printf("middle interpvma translate\n"); */
    *start = TranslationTableWalkSuppliedPGD(vm_start, pgdPaddr);
    /* printf("after interpvma translate\n"); */
    *size = ((uint64_t*)((char*)memdev+vma))[1] - ((uint64_t*)((char*)memdev+vma))[0];
    uint64_t nextVaddr = ((uint64_t*)((char*)memdev+vma))[2];
    //TODO
    /* *next = GetPhysAddr(nextVaddr); */
    *next = intro_virt_to_phys(nextVaddr);
    /* *next = isKernelTask ? intro_virt_to_phys(nextVaddr) : TranslationTableWalk(nextVaddr); */
    *flags = ((uint64_t*)((char*)memdev+vma))[10];
}

void CrawlVMAs(uint64_t vma, uint64_t pgdPaddr, uint8_t* rodataDigests, int* numRodataDigests, bool isKernelTask)
{
    uint64_t start;
    uint64_t size;
    uint64_t next;
    uint64_t flags;
    InterpVMA(vma, &start, &size, &next, &flags, pgdPaddr, isKernelTask);

    if( start + size < 0x8001000
        && ((char*)memdev+start)[0] == 0x7f
        && ((char*)memdev+start)[1] == 'E' 
        && ((char*)memdev+start)[2] == 'L' 
        && ((char*)memdev+start)[3] == 'F' )
    {
        printf("found an elf\n");
        InterpretElfHeader(start, pgdPaddr, rodataDigests, numRodataDigests, isKernelTask);
        printf("goodbye elf\n");
    }

    if(next != 0)
    {
        CrawlVMAs(next, pgdPaddr, rodataDigests, numRodataDigests, isKernelTask);
    }
}

bool InterpretMemory(uint64_t task, uint8_t* rodataDigest, bool isKernelTask)
{
    /* There probably won't be more than 100 rodata sections? */
    uint8_t rodataDigests[64 * 100];
    int numRodataDigests = 0;

    uint64_t mmAddrLoc = task + 1024;
    uint64_t mmAddr = ((uint64_t*)((char*)memdev+mmAddrLoc))[0];
    if(mmAddr==0)
    {
        // try active_mm instead
        mmAddr = ((uint64_t*)((char*)memdev+mmAddrLoc+8))[0];
        if(mmAddr==0)
        {
            // this task has no memory to interpret
            return false;
        }
        printf("defaulting to active_mm\n");
    }
    // TODO
    /* uint64_t mm = isKernelTask ? intro_virt_to_phys(mmAddr) : TranslationTableWalk(mmAddr); */
    /* uint64_t mm = GetPhysAddr(mmAddr); */
    uint64_t mm = intro_virt_to_phys(mmAddr);

    uint64_t pgdVaddr = ((uint64_t*)((char*)memdev+mm+64))[0];
    // TODO
    /* uint64_t pgdPaddr = isKernelTask ? intro_virt_to_phys(pgdVaddr) : TranslationTableWalk(pgdVaddr); */
    /* uint64_t pgdPaddr = GetPhysAddr(pgdVaddr); */
    uint64_t pgdPaddr = intro_virt_to_phys(pgdVaddr);

    uint64_t mmapAddr = ((uint64_t*)((char*)memdev+mm))[0];
    // TODO
    /* uint64_t mmap = isKernelTask ? intro_virt_to_phys(mmapAddr) : TranslationTableWalk(mmapAddr); */
    /* uint64_t mmap = GetPhysAddr(mmapAddr); */
    uint64_t mmap = intro_virt_to_phys(mmapAddr);

    CrawlVMAs(mmap, pgdPaddr, &rodataDigests, &numRodataDigests, isKernelTask);

    HashHashes(&rodataDigests, numRodataDigests, rodataDigest);
    printf("We hashed %d rodata sections. Then we hashed their ordered concatentation.\n", numRodataDigests);

    return true;
}

void GetPID(uint64_t task, int* myPid)
{
    int myPidLoc = task + 1200;
    *myPid =((int*)((char*)memdev+myPidLoc))[0];
}

void GetPIDs(uint64_t task, int* myPid, int* parentPid)
{
    GetPID(task, myPid);
    uint64_t parentAddrLoc = task + 1216;
    uint64_t parentAddr = ((uint64_t*)((char*)memdev+parentAddrLoc))[0];
    uint64_t parent = GetPhysAddr(parentAddr);
    if(ValidateTaskStruct(parent))
    {
        GetPID(parent, parentPid);
    }
}

void InterpretCred(uint64_t task, struct cred* cred)
{
    uint64_t credAddrLoc = task + 1640 - 8 - 8; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memdev+credAddrLoc))[0];
    uint64_t credPaddr = GetPhysAddr(credVaddr);
    
    cred->usage = ((int*)((char*)memdev+credPaddr))[0];
    cred->uid = ((int*)((char*)memdev+credPaddr+4))[0];
    cred->gid = ((int*)((char*)memdev+credPaddr+8))[0];
    cred->suid = ((int*)((char*)memdev+credPaddr+12))[0];
    cred->sgid = ((int*)((char*)memdev+credPaddr+16))[0];
    cred->euid = ((int*)((char*)memdev+credPaddr+20))[0];
    cred->egid = ((int*)((char*)memdev+credPaddr+24))[0];
    cred->fsuid = ((int*)((char*)memdev+credPaddr+28))[0];
    cred->fsgid = ((int*)((char*)memdev+credPaddr+32))[0];
    cred->securebits = ((int*)((char*)memdev+credPaddr+36))[0];
    cred->cap_inheritable = ((uint64_t*)((char*)memdev+credPaddr+40))[0];
    cred->cap_permitted = ((uint64_t*)((char*)memdev+credPaddr+48))[0];
    cred->cap_effective = ((uint64_t*)((char*)memdev+credPaddr+56))[0];
    cred->cap_bset = ((uint64_t*)((char*)memdev+credPaddr+64))[0];
    cred->cap_ambient = ((uint64_t*)((char*)memdev+credPaddr+72))[0];
    cred->jit_keyring = ((char*)memdev+credPaddr+80)[0];
    cred->session_keyring = ((uint64_t*)((char*)memdev+credPaddr+88))[0];
    cred->process_keyring = ((uint64_t*)((char*)memdev+credPaddr+96))[0];
    cred->thread_keyring = ((uint64_t*)((char*)memdev+credPaddr+104))[0];
    cred->request_key_auth = ((uint64_t*)((char*)memdev+credPaddr+112))[0];
    cred->security = ((uint64_t*)((char*)memdev+credPaddr+120))[0];
    cred->user_struct = ((uint64_t*)((char*)memdev+credPaddr+128))[0];
    cred->user_namespace = ((uint64_t*)((char*)memdev+credPaddr+136))[0];
    cred->group_info = ((uint64_t*)((char*)memdev+credPaddr+144))[0];
}

void InterpretTaskStruct(uint64_t inputAddress, uint64_t* children, uint64_t* sibling)//, uint64_t* parent)
{
    int childLoc = inputAddress + 1232;
    uint64_t firstChild = ((uint64_t*)((char*)memdev+childLoc))[0];
    *children = GetPhysAddr(firstChild)+392-1640;

    // get a sibling pointer
    int siblingLoc = inputAddress + 1232 + 16;
    uint64_t leftSibling = ((uint64_t*)((char*)memdev+siblingLoc))[0];
    *sibling = GetPhysAddr(leftSibling)+392-1640;
}

void CrawlProcesses(uint64_t task, uint64_t leadSibling, struct TaskMeasurement* results, int taskID)
{
    // verify we're a real task
    if(!ValidateTaskStruct(task))
    {
        return;
    }

    // Collect a Measurement
    /* ScanTaskStruct(task); */
    GetTaskName(task, &results[taskID].name);
    GetPIDs(task, &results[taskID].myPid, &results[taskID].parentPid);
    InterpretCred(task, &results[taskID].cred);

    printf("interp mem for %s\n", &results[taskID].name);
    /* ScanTaskStruct(task); */
    bool hasMemory = false;

    /* /1* if(strcmp(&results[taskID].name, "useram")==0) *1/ */
    /* if(strcmp(&results[taskID].name, "init")!=0 && */
    /*    strcmp(&results[taskID].name, "rcS")!=0 && */
    /*    strcmp(&results[taskID].name, "klogd")!=0 && */
    /*    strcmp(&results[taskID].name, "S90cross_vm_ini")!=0 && */
    /*    strcmp(&results[taskID].name, "mkdir")!=0 && */
    /*    strcmp(&results[taskID].name, "adduser")!=0 && */
    /*    strcmp(&results[taskID].name, "getty")!=0 && */
    /*    strcmp(&results[taskID].name, "sh")!=0 && */
    /*    strcmp(&results[taskID].name, "syslogd")!=0) */
    /* { */
    /*     //ScanTaskStruct(task); */
    /*     hasMemory = InterpretMemory(task, &results[taskID].rodataDigest); */
    /* } */
    /* if(strcmp(&results[taskID].name, "init")==0) */
    /* { */
    /*     bool isKernelTask = results[taskID].cred.uid==0 && results[taskID].cred.suid==0; */
    /*     hasMemory = InterpretMemory(task, &results[taskID].rodataDigest, isKernelTask); */
    /* } */

    bool isKernelTask = results[taskID].cred.uid==0 && results[taskID].cred.suid==0;
    hasMemory = InterpretMemory(task, &results[taskID].rodataDigest, isKernelTask);

    // prepare to crawl
    uint64_t myChild;
    uint64_t mySibling;
    InterpretTaskStruct(task, &myChild, &mySibling);

    // crawl a la breadth-first-search
    if(mySibling != leadSibling)
    {
        CrawlProcesses(mySibling, task, results, ++taskID);
    }
    CrawlProcesses(myChild, myChild, results, ++taskID);
}

void MeasureProcesses()
{
    printf("DEBUG: Measurement: Beginning process measurement.\n");

    int numTasks = 100;
    struct TaskMeasurement* taskMsmts = calloc(numTasks,sizeof(struct TaskMeasurement));
    for(int i=0; i<numTasks; i++)
    {
        for(int j=0; j<64; j++)
        {
            taskMsmts[i].rodataDigest[j] = 0;
        }
    }


    int numTasksCollected = 0;

    uint64_t init_task_ptr = (uint64_t)INIT_TASK_ADDR;
    CrawlProcesses(init_task_ptr, init_task_ptr, taskMsmts, numTasksCollected);

    for(int i=0; i<100; i++)
    {

        /* //if(taskMsmts[i].name[0] != '\0') */
        /* if(strcmp(taskMsmts[i].name, "useram")==0) */
        /* { */
        /*     PrintTaskEvidence(&taskMsmts[i]); */
        /* } */

        if(!IsDigestEmpty(taskMsmts[i].rodataDigest))
        {
            PrintTaskEvidence(&taskMsmts[i]);
        }
    }

    //ScanTaskStruct(init_task_ptr);
}

