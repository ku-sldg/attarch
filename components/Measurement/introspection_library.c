/*
 * An introspection library for Linux 4.9.307
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#define RAM_BASE 0x40000000

// TODO collect this list_head address as part of the build process
#define LIST_HEAD_ADDR 0xFB61E0
#define INIT_TASK_ADDR 0xF92280

void introLog(int args, ...)
{
    if(true)
    {
        printf("DEBUG: Introspection: ");
        va_list ptr;
        va_start(ptr, args);
        for(int i=0; i<args; i++)
        {
            printf("%s", va_arg(ptr,char*));
        }
        printf("\n");
    }
}

uint64_t intro_virt_to_phys(uint64_t virtaddr)
{
    //return((virtaddr & 0xFFFFFFFF) + 0x40000000);
    return(virtaddr & 0xFFFFFFFF);
}

bool IsThisAValidModuleMeasurement(char* moduleName)
{
    for(int i=0; i<56; i++)
    {
        if(moduleName[i] != '\0')
        {
            // an invalid (unused) module name should be completely
            // zeroed out
            return true;
        }
    }
    return false;
}

uint64_t TranslationTableWalk(uint64_t inputAddr)
{
    bool TTWalkDebug = false;

    uint64_t PGDindex = (inputAddr & 0x0000FF8000000000) >> 39;
    uint64_t PUDindex = (inputAddr & 0x0000007FC0000000) >> 30;
    uint64_t PMDindex = (inputAddr & 0x000000003FE00000) >> 21;
    uint64_t PTEindex = (inputAddr & 0x00000000001FF000) >> 12;
    uint64_t PAGindex = (inputAddr & 0x0000000000000FFF) >>  0;

    if(TTWalkDebug)
    {
        printf("input %016X,\nPGDindex %016X,\nPUDindex %016X,\nPMDindex %016X,\nPTEindex %016X\n", inputAddr, PGDindex, PUDindex, PMDindex, PTEindex); 
        printf("PGDindex %d,\nPUDindex %d,\nPMDindex %d,\nPTEindex %X\n", PGDindex, PUDindex, PMDindex, PTEindex); 
    }
    char* PGDTablePtr = ((char*)memdev)+0x4113D000 - RAM_BASE;
    uint64_t* PGDTable = (uint64_t*)PGDTablePtr;
    uint64_t pudAddr = (PGDTable[PGDindex] & 0x00000000FFFFF000) - RAM_BASE;

    if(TTWalkDebug)
    {
        printf("Here is the PGD\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%016X\n", PGDTable[i]);
        }
        printf("Get PUD base address from PGD\n");
        printf("pudAddr is %016X\n", pudAddr);
    }

    // TODO investigate these bits we drop from every table entry
    char* pudTablePtr = ((char*)memdev)+pudAddr;
    uint64_t* PUDTable = (uint64_t*)pudTablePtr;
    uint64_t pmdAddr = (PUDTable[PUDindex] & 0x00000000FFFFF000) - RAM_BASE;

    if(TTWalkDebug)
    {
        printf("Here is the PUD\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%X: %016X\n", i, PUDTable[i]);
        }
        printf("pmdAddr is %016X\n", pmdAddr);
    }

    char* pmdTablePtr = ((char*)memdev)+pmdAddr;
    uint64_t* pmdTable = (uint64_t*)pmdTablePtr;
    uint64_t pteAddr = (pmdTable[PMDindex] & 0x00000000FFFFF000) - RAM_BASE;

    if(TTWalkDebug)
    {
        printf("Here is the pmd\n");
        for(int i=0; i<0x4; i++)
        {
            printf("%X: %016X\n", i, pmdTable[i]);
        }
        printf("pteAddr is %016X\n", pteAddr);
    }

    char* pteTablePtr = ((char*)memdev)+pteAddr;
    uint64_t* pteTable = (uint64_t*)pteTablePtr;
    uint64_t offsetAddr = (pteTable[PTEindex] & 0x00000000FFFFF000) - RAM_BASE;
    uint64_t finalPaddr = offsetAddr | PAGindex;

    if(TTWalkDebug)
    {
        printf("Here is the pte at 1C2\n");
        for(int i=0x1C2; i<0x1C6; i++)
        {
            printf("%X: %016X\n", i, pteTable[i]);
        }
        printf("offsetAddr is %016X\n", offsetAddr);
        printf("Output Address is %016X\n", finalPaddr + RAM_BASE);
        printf("\nTable walk complete\n");
    }

    return finalPaddr;
}

struct module_layout {
    uint64_t base;
    unsigned int size;
    unsigned int text_size;
    unsigned int ro_size;
    unsigned int ro_after_init_size;
};

struct module_layout GetModuleLayoutFromListHead(int physAddr)
{
    int index = physAddr;
    index += 16; // skip list_head
    index += 56; // skip name
    index += 96; //skip mkobj
    index += 8; // skio modinfo_attrs
    index += 8; // skip version
    index += 8; // skip srcversion
    index += 8; // skip holders_dir
    index += 8; // skip syms
    index += 8; // skip crcs
    index += 4; // skip num_syms
    index += 40; // skip struct mutex
    index += 8; // skip kp
    index += 4; // num_kp
    index += 4; // num_gpl_syms
    index += 8; // gpl_syms
    index += 8; // gpl_crcs
    index += 1; //async_probe_requested
    index += 3; // to fill a 4-byte buffer
    index += 8; // gpl_future_syms
    index += 8; // gpl_future_crcs
    index += 4; // num_gpl_future_syms
    index += 4; // num_exentries
    index += 8; // extable
    index += 8; // (*init*(void)

    //by inspection
    //printerate(physAddr + 47 * 8, 3);
    struct module_layout thisModule;
    thisModule.base = ((uint64_t*)((char*)memdev+physAddr+47*8))[0];
    thisModule.size = ((unsigned int*)((char*)memdev+physAddr+47*8))[2];
    thisModule.text_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[3];
    thisModule.ro_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[4];
    thisModule.ro_after_init_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[5];
    return thisModule;
}

void InterpretKernelModule(uint64_t inputAddress, uint8_t* rodataDigest, char* name)
{
    bool IKMDebug = false;
    if(IKMDebug)
    {
        printf("Module Address: %016X\n", inputAddress);
    }

    for(int j=16; j<56+16; j++)
    {
        name[j-16] = ((char*)memdev)[inputAddress+j];
    }

    char msg[13] = "Found Module ";
    introLog(2, msg, name);
    /*
    printf("DEBUG: Measurement: Found Module: ");
    for(int j=0; j<56; j++)
    {
        printf("%c", name[j]);
    }
    */
    printf("\n");

    struct module_layout thisModuleLayout = GetModuleLayoutFromListHead((int)inputAddress);
    uint64_t basePtr = TranslationTableWalk(thisModuleLayout.base);

    if(IKMDebug)
    {
        printf("base: %016X\n", thisModuleLayout.base);
        printf("size: %08X\n", thisModuleLayout.size);
        printf("text size: %08X\n", thisModuleLayout.text_size);
        printf("ro size: %08X\n", thisModuleLayout.ro_size);
        printf("ro after init size: %08X\n", thisModuleLayout.ro_after_init_size);
        printf("base paddr: %016X\n", basePtr);
    }

    // collect the read-only data
    uint8_t* rodata = malloc(thisModuleLayout.ro_size);
    for(int i=0; i<thisModuleLayout.ro_size; i++)
    {
        rodata[i] = ((char*)memdev)[basePtr+i];
    }

    // digest the read-only data with Sha512 from HACL
    Hacl_Hash_SHA2_hash_512(rodata, thisModuleLayout.ro_size, rodataDigest);
    free(rodata);

    // can print out the rodata here to see strings from the source
    //printerateChars(basePtr, thisModuleLayout.ro_size);
}

void MeasureKernelModules(uint8_t** module_digests, char** module_names)
{
    bool MKMDebug = false;
    printf("DEBUG: Measurement: Beginning kernel module measurement.\n");
    if(MKMDebug)
    {
        printf("Collecting module pointers...\n");
    }
    /* modulePtrs is a list of offsets into memdev that refer to kernel
    ** modules. They are physical memory addresses with the RAM_BASE
    ** already subtracted. Assume there are no more than 128 modules.
    */
    uint64_t modulePtrs[128];
    for(int i=0; i<128; i++)
    {
        modulePtrs[i] = 0;
    }
    int numModulePtrs = 0;
    uint64_t* list_head_ptr = (uint64_t*)(((char*)memdev)+LIST_HEAD_ADDR);
    uint64_t module_pointer = TranslationTableWalk(list_head_ptr[0]);
    while(module_pointer != LIST_HEAD_ADDR)
    {
        modulePtrs[numModulePtrs] = module_pointer;
        numModulePtrs++;
        char* modBytePtr = ((char*)memdev)+module_pointer;
        uint64_t* modLongPtr = (uint64_t*)modBytePtr;
        module_pointer = TranslationTableWalk(modLongPtr[0]);
    }
    if(MKMDebug)
    {
        printf("Collecting digests over module rodata...\n");
    }
    for(int i=0; i<128; i++)
    {
        if(modulePtrs[i] != 0)
        {
            InterpretKernelModule(modulePtrs[i], module_digests[i], module_names[i]);
        }
    }
}

void introspectScan(int* head, int size, char* name)
{
    printf("%s: ", name);
    //for(int i=0; i<size; i++)
    for(int i=size-1; i>=0; i--)
    {
        printf("%02X",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanChar(int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    //for(int i=size; i>0; i--)
    {
        printf("%c",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanInt(int* head, int size, char* name)
{
    printf("%s: ", name);
    int* val = (int*)((char*)memdev+*head);
    printf("%d", *val);
    printf("\n");
    *head += size;
}
void scanTaskStruct()
{
    //int index = inputAddress;
    int index = 0;
    introspectScan(&index, 8, "volatile  long , state");
    introspectScan(&index, 8, "void *, stack");
    introspectScan(&index, 4, "atomic_t , usage");
    introspectScan(&index, 4, "unsigned int , flags");
    introspectScan(&index, 4, "unsigned int , ptrace");
    introspectScan(&index, 8, "struct llist_node , wake_entry");
    introspectScan(&index, 4, "int , on_cpu");
    introspectScan(&index, 4, "unsigned int , wakee_flips");
    introspectScan(&index, 8, "unsigned long , wakee_flip_decay_ts");
    introspectScan(&index, 8, "struct task_struct *, last_wakee");
    introspectScan(&index, 4, "int , wake_cpu");
    introspectScan(&index, 4, "int , on_rq");
    introspectScanInt(&index, 4, "int , prio");
    introspectScanInt(&index, 4, "int , static_prio");
    introspectScanInt(&index, 4, "int , normal_prio");
    introspectScanInt(&index, 4, "unsigned int , rt_priority");
    //TODO explain:
    introspectScan(&index, 48, "compensation");
    introspectScan(&index, 8, "const struct sched_class *, sched_class");
    introspectScan(&index, 512, "struct sched_entity , se");
    introspectScan(&index, 48, "struct sched_rt_entity , rt");
    introspectScan(&index, 8, "struct task_group *, sched_task_group");
    introspectScan(&index, 160, "struct sched_dl_entity , dl");
    introspectScan(&index, 8, "struct hlist_head , preempt_notifiers");
    introspectScan(&index, 4, "unsigned int , btrace_seq");
    introspectScanInt(&index, 4, "unsigned int , policy");
    introspectScanInt(&index, 4, "int , nr_cpus_allowed");
    introspectScan(&index, 32, "cpumask_t , cpus_allowed");
    introspectScan(&index, 32, "struct sched_info , sched_info");
    introspectScan(&index, 16, "struct list_head , tasks");
    introspectScan(&index, 40, "struct plist_node , pushable_tasks");
    introspectScan(&index, 24, "struct rb_node , pushable_dl_tasks");
    introspectScan(&index, 8, "struct mm_struct *, mm");
    introspectScan(&index, 8, "struct mm_struct *, active_mm");
    introspectScan(&index, 8, "u64 , vmacache_seqnum");
    introspectScan(&index, 8, "struct vm_area_struct *, vmacache1");
    introspectScan(&index, 8, "struct vm_area_struct *, vmacache2");
    introspectScan(&index, 8, "struct vm_area_struct *, vmacache3");
    introspectScan(&index, 8, "struct vm_area_struct *, vmacache4");
    introspectScan(&index, 20, "struct  task_rss_stat   , rss_stat");
    introspectScanInt(&index, 4, "int , exit_state");
    introspectScanInt(&index, 4, "int , exit_code");
    introspectScanInt(&index, 4, "int , exit_signal");
    introspectScanInt(&index, 4, "int , pdeath_signal");
    introspectScan(&index, 8, "unsigned long , jobctl");
    introspectScan(&index, 4, "unsigned int , personality");
    introspectScan(&index, 4, "unsigned int , bits1");
    introspectScan(&index, 4, "unsigned int , bits2");
    introspectScan(&index, 8, "unsigned long , atomic_flags");
    introspectScan(&index, 48, "struct restart_block , restart_block");
    introspectScanInt(&index, 4, "pid_t , pid");
    introspectScanInt(&index, 4, "pid_t , tgid");
    introspectScan(&index, 8, "unsigned long , stack_canary");
    introspectScan(&index, 8, "struct task_struct __rcu *, real_parent");
    introspectScan(&index, 8, "struct task_struct __rcu *, parent");
    introspectScan(&index, 16, "struct list_head , children");
    introspectScan(&index, 16, "struct list_head , sibling");
    introspectScan(&index, 8, "struct task_struct *, group_leader");
    introspectScan(&index, 16, "struct list_head , ptraced");
    introspectScan(&index, 16, "struct list_head , ptrace_entry");
    introspectScan(&index, 24, "struct pid_link , pids1");
    introspectScan(&index, 24, "struct pid_link , pids2");
    introspectScan(&index, 24, "struct pid_link , pids3");
    introspectScan(&index, 16, "struct list_head , thread_group");
    introspectScan(&index, 16, "struct list_head , thread_node");
    introspectScan(&index, 8, "struct completion *, vfork_done");
    introspectScan(&index, 8, "int __user *, set_child_tid");
    introspectScan(&index, 8, "int __user *, clear_child_tid");
    introspectScan(&index, 8, "cputime_t , utime");
    introspectScan(&index, 8, "cputime_t , stime");
    introspectScan(&index, 8, "cputime_t , utimescaled");
    introspectScan(&index, 8, "cputime_t , stimescaled");
    introspectScan(&index, 8, "cputime_t , gtime");
    introspectScan(&index, 24, "struct prev_cputime , prev_cputime");
    introspectScan(&index, 8, "unsigned long , nvcsw");
    introspectScan(&index, 8, "unsigned long , nivcsw");
    introspectScan(&index, 8, "u64 , start_time");
    introspectScan(&index, 8, "u64 , real_start_time");
    introspectScan(&index, 8, "unsigned long , min_flt");
    introspectScan(&index, 8, "unsigned long , maj_flt");
    introspectScan(&index, 24, "struct task_cputime , cputime_expires");
    introspectScan(&index, 16, "struct list_head , cpu_timers1");
    introspectScan(&index, 16, "struct list_head , cpu_timers2");
    introspectScan(&index, 16, "struct list_head , cpu_timers3");
    introspectScan(&index, 8, "const struct cred __rcu *, ptracer_cred");
    introspectScan(&index, 8, "const struct cred __rcu *, real_cred");
    introspectScan(&index, 8, "const struct cred __rcu *, cred");
    introspectScanChar(&index, 16, "char , comm");
    introspectScan(&index, 8, "struct nameidata *, nameidata");
    introspectScan(&index, 8, "struct sysv_sem , sysvsem");
    introspectScan(&index, 16, "struct sysv_shm , sysvshm");
    introspectScan(&index, 8, "unsigned long , last_switch_count");
    introspectScan(&index, 8, "struct fs_struct *, fs");
    introspectScan(&index, 8, "struct files_struct *, files");
    introspectScan(&index, 8, "struct nsproxy *, nsproxy");
    introspectScan(&index, 8, "struct signal_struct *, signal");
    introspectScan(&index, 8, "struct sighand_struct *, sighand");
    introspectScan(&index, 8, "sigset_t , blocked");
    introspectScan(&index, 8, "sigset_t , real_blocked");
    introspectScan(&index, 8, "sigset_t , saved_sigmask");
    introspectScan(&index, 24, "struct sigpending , pending");
    introspectScan(&index, 8, "unsigned long , sas_ss_sp");
    introspectScan(&index, 8, "size_t , sas_ss_size");
    introspectScan(&index, 4, "unsigned , sas_ss_flags");
    introspectScan(&index, 8, "struct callback_head *, task_works");
    introspectScan(&index, 8, "struct audit_context *, audit_context");
    introspectScan(&index, 4, "kuid_t , loginuid");
    introspectScan(&index, 4, "unsigned int , sessionid");
    introspectScan(&index, 16, "struct seccomp , seccomp");
    introspectScan(&index, 8, "u64 , parent_exec_id");
    introspectScan(&index, 8, "u64 , self_exec_id");
    introspectScan(&index, 4, "spinlock_t , alloc_lock");
    introspectScan(&index, 4, "raw_spinlock_t , pi_lock");
    introspectScan(&index, 8, "struct wake_q_node , wake_q");
    introspectScan(&index, 8, "struct rb_root , pi_waiters");
    introspectScan(&index, 8, "struct rb_node *, pi_waiters_leftmost");
    introspectScan(&index, 8, "struct rt_mutex_waiter *, pi_blocked_on");
    introspectScan(&index, 8, "void *, journal_info");
    introspectScan(&index, 8, "struct bio_list *, bio_list");
    introspectScan(&index, 8, "struct blk_plug *, plug");
    introspectScan(&index, 8, "struct reclaim_state *, reclaim_state");
    introspectScan(&index, 8, "struct backing_dev_info *, backing_dev_info");
    introspectScan(&index, 8, "struct io_context *, io_context");
    introspectScan(&index, 8, "unsigned long , ptrace_message");
    introspectScan(&index, 8, "siginfo_t *, last_siginfo");
    introspectScan(&index, 56, "struct task_io_accounting , ioac");
    introspectScan(&index, 8, "u64 , acct_rss_mem1");
    introspectScan(&index, 8, "u64 , acct_vm_mem1");
    introspectScan(&index, 8, "cputime_t , acct_timexpd");
    introspectScan(&index, 8, "nodemask_t , mems_allowed");
    introspectScan(&index, 4, "seqcount_t , mems_allowed_seq");
    introspectScan(&index, 4, "int , cpuset_mem_spread_rotor");
    introspectScan(&index, 4, "int , cpuset_slab_spread_rotor");
    introspectScan(&index, 8, "struct css_set __rcu *, cgroups");
    introspectScan(&index, 16, "struct list_head , cg_list");
    introspectScan(&index, 8, "struct robust_list_head __user *, robust_list");
    introspectScan(&index, 8, "struct compat_robust_list_head __user *, compat_robust_list");
    introspectScan(&index, 16, "struct list_head , pi_state_list");
    introspectScan(&index, 8, "struct futex_pi_state *, pi_state_cache");
    introspectScan(&index, 40, "struct mutex , futex_exit_mutex");
    introspectScan(&index, 4, "unsigned int , futex_state");
    introspectScan(&index, 8, "struct perf_event_context *, perf_event_ctxp1");
    introspectScan(&index, 8, "struct perf_event_context *, perf_event_ctxp2");
    introspectScan(&index, 40, "struct mutex , perf_event_mutex");
    introspectScan(&index, 16, "struct list_head , perf_event_list");
    introspectScan(&index, 16, "struct rcu_head , rcu");
    introspectScan(&index, 8, "struct pipe_inode_info *, splice_pipe");
    introspectScan(&index, 16, "struct page_frag , task_frag");
    introspectScan(&index, 8, "struct task_delay_info *, delays");
    introspectScan(&index, 4, "int , nr_dirtied");
    introspectScan(&index, 4, "int , nr_dirtied_pause");
    introspectScan(&index, 8, "unsigned long , dirty_paused_when");
    introspectScan(&index, 8, "u64 , timer_slack_ns");
    introspectScan(&index, 8, "u64 , default_timer_slack_ns");
    introspectScan(&index, 4, "int , curr_ret_stack");
    introspectScan(&index, 8, "struct ftrace_ret_stack *, ret_stack");
    introspectScan(&index, 8, "unsigned long long , ftrace_timestamp");
    introspectScan(&index, 4, "atomic_t , trace_overrun");
    introspectScan(&index, 4, "atomic_t , tracing_graph_pause");
    introspectScan(&index, 8, "unsigned long , trace");
    introspectScan(&index, 8, "unsigned long , trace_recursion");
    introspectScan(&index, 8, "struct mem_cgroup *, memcg_in_oom");
    introspectScan(&index, 4, "gfp_t , memcg_oom_gfp_mask");
    introspectScan(&index, 4, "int , memcg_oom_order");
    introspectScan(&index, 4, "unsigned int , memcg_nr_pages_over_high");
    introspectScan(&index, 4, "unsigned  int   , sequential_io");
    introspectScan(&index, 4, "unsigned  int   , sequential_io_avg");
    introspectScan(&index, 4, "int , pagefault_disabled");
    introspectScan(&index, 8, "struct task_struct *, oom_reaper_list");
    introspectScan(&index, 960, "struct thread_struct , thread");
}

void PrintTaskName(uint64_t task)
{
    char* name = calloc(1, 16);
    int nameLoc = task + 1640;
    for(int i=0; i<16; i++)
    {
        name[i] = ((char*)memdev+nameLoc)[i];
        if(name[i] > 127)
        {
            // this task is considered invalid because it's name contained a
            // non-ascii character
            return;
        }
    }
    introLog(2, "Task Name: ", name);
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

void InterpretTaskStruct(uint64_t inputAddress, uint64_t* children, uint64_t* sibling)
{
    // get a child pointer
    int childLoc = inputAddress + 1232;
    uint64_t firstChild = ((uint64_t*)((char*)memdev+childLoc))[0];
    *children = intro_virt_to_phys(firstChild)+392-1640;
    if(!ValidateTaskStruct(*children))
    {
        *children = TranslationTableWalk(firstChild)+392-1640;
    }

    // get a sibling pointer
    int siblingLoc = inputAddress + 1232 + 16;
    uint64_t leftSibling = ((uint64_t*)((char*)memdev+siblingLoc))[0];
    *sibling = intro_virt_to_phys(leftSibling)+392-1640;
    if(!ValidateTaskStruct(*sibling))
    {
        *sibling = TranslationTableWalk(leftSibling)+392-1640;
    }
}

void CrawlProcesses(uint64_t task, uint64_t leadSibling)
{
    // verify we're a real task
    if(!ValidateTaskStruct(task))
    {
        return;
    }

    // do some job
    PrintTaskName(task);

    // prepare to crawl
    uint64_t myChild;
    uint64_t mySibling;
    InterpretTaskStruct(task, &myChild, &mySibling);

    // crawl a la breadth-first-search
    if(mySibling != leadSibling)
    {
        CrawlProcesses(mySibling, task);
    }
    CrawlProcesses(myChild, myChild);
}

void MeasureProcesses()
{
    printf("DEBUG: Measurement: Beginning process measurement.\n");
    uint64_t init_task_ptr = (uint64_t)INIT_TASK_ADDR;
    CrawlProcesses(init_task_ptr, init_task_ptr);
}

