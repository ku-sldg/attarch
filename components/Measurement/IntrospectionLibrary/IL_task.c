/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#include "IL_struct_interp.c"
#include "IL_elf_header.c"
#include "../hash.h"

/* C program for array implementation of queue */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct cred {
    int usage;
    int uid, gid, suid, sgid, euid, egid, fsuid, fsgid;
    int securebits;
    uint64_t cap_inheritable, cap_permitted, cap_effective, cap_bset, cap_ambient;
    char jit_keyring;
    uint64_t session_keyring, process_keyring, thread_keyring, request_key_auth, security;
    uint64_t user_struct, user_namespace, group_info;
};

typedef struct TaskMeasurement
{
    uint64_t paddr;
    struct TaskMeasurement* parent;
    struct TaskMeasurement* real_parent;
    struct TaskMeasurement** children;

    char* name;
    int uid;
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
    struct cred cred;
    char rodataDigest[64];
} TaskMeasurement;


/* A structure to represent a queue */
struct Queue {
    int front, rear, size;
    unsigned capacity;
    TaskMeasurement** array;
};

/* function to create a queue of given capacity. It initializes size of queue
** as 0 */
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (TaskMeasurement**)malloc(queue->capacity * sizeof(TaskMeasurement*));
    return queue;
}

/* Queue is full when size becomes equal to the capacity */
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

/* Queue is empty when size is 0 */
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

/* Function to add an item to the queue. It changes rear and size */
void enqueue(struct Queue* queue, TaskMeasurement* item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

/* Function to remove an item from queue. It changes front and size */
TaskMeasurement* dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
    {
        return NULL;
    }
    TaskMeasurement* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}




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
    printf("\n");
}

void GetTaskName(uint64_t task, char* name)
{
    int nameLoc = task + 1640;
    for(int i=0; i<16; i++)
    {
        name[i] = ((char*)memdev+nameLoc)[i];
    }
}

char* GetTaskNamePointer(uint64_t task)
{
    return ((char*)memdev) + task + 1640;
}

bool ValidateTaskStruct(uint64_t task)
{
    if(task > 0x8001000)
    {
        /* printf("out of range\n"); */
        return false;
    }
    int nameLoc = task + 1640;
    if(((char*)memdev+nameLoc)[0] == '\0')
    {
        /* printf("null name\n"); */
        return false;
    }
    for(int i=0; i<16; i++)
    {
        if(((char*)memdev+nameLoc)[i] > 127)
        {
            /* printf("illegal characters in name\n"); */
            /* int thisShouldBeNameData = nameLoc; */
            /* introspectScan(&thisShouldBeNameData, 64, "offending task?:\n"); */
            return false;
        }
    }
    return true;
}

void InterpVMA(uint64_t vma, uint64_t* start, uint64_t* size, uint64_t* next, uint64_t* flags, uint64_t pgdPaddr)
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
    /* We need this conditional, but we're not entirely sure why */ 
    if((unsigned long)0xF0000000 < (unsigned long)vm_start)
    {
        *start = intro_virt_to_phys(vm_start);
    }
    else
    {
        *start = TranslationTableWalkSuppliedPGD(vm_start, pgdPaddr);
    }
    *size = ((uint64_t*)((char*)memdev+vma))[1] - ((uint64_t*)((char*)memdev+vma))[0];
    uint64_t nextVaddr = ((uint64_t*)((char*)memdev+vma))[2];
    uint64_t prevVaddr = ((uint64_t*)((char*)memdev+vma))[3];
    *flags = ((uint64_t*)((char*)memdev+vma))[10];
    if(nextVaddr == 0)
    {
        *next = 0;
        return;
    }
    // we intend for this to ensure impossible address translations do not take
    // place...
    if((long)nextVaddr < (long)0xFFFF80000000)
    {
        *next = TranslateVaddr(nextVaddr);
    }
    else
    {
        printf("walk supplied pgd\n");
        *next = TranslationTableWalkSuppliedPGD(nextVaddr, pgdPaddr);
    }
}

void CrawlVMAs(char* name, uint64_t vma, uint64_t pgdPaddr, uint8_t* rodataDigest)
{
    uint64_t start;
    uint64_t size;
    uint64_t next;
    uint64_t flags;
    InterpVMA(vma, &start, &size, &next, &flags, pgdPaddr);
    if( start + size < 0x8001000
            && ((char*)memdev+start)[0] == 0x7f
            && ((char*)memdev+start)[1] == 'E' 
            && ((char*)memdev+start)[2] == 'L' 
            && ((char*)memdev+start)[3] == 'F' )
    {
        if(TryMeasureElfRodata(start, pgdPaddr, rodataDigest))
        {
            // we got a good digest
            /* printf("Measured ELF rodata for task %s\n", name); */
            // we expect a task to have at most one ELF header in memory, so we
            // can return right away.
            return;
        }
    }
    if(next != 0)
    {
        CrawlVMAs(name, next, pgdPaddr, rodataDigest);
    }
}
void InterpretMemory(uint64_t task, char* name, uint8_t* rodataDigest)
{
    uint64_t mmAddrLoc = task + 1024;
    uint64_t mmAddr = ((uint64_t*)((char*)memdev+mmAddrLoc))[0];
    if(mmAddr==0)
    {
        mmAddr = ((uint64_t*)((char*)memdev+mmAddrLoc+8))[0];
        if(mmAddr==0)
        {
            return;
        }
    }
    uint64_t mm = TranslateVaddr(mmAddr);
    uint64_t pgdVaddr = ((uint64_t*)((char*)memdev+mm+64))[0];
    uint64_t pgdPaddr = TranslateVaddr(pgdVaddr);
    uint64_t mmapAddr = ((uint64_t*)((char*)memdev+mm))[0];
    uint64_t mmap = TranslateVaddr(mmapAddr);
    // these linked lists are apparently null terminated on the ends,
    // so here we put ourselves at the "leftmost" position,
    // before we crawl strictly to the right
    uint64_t prevVaddr = ((uint64_t*)((char*)memdev+mmap))[3];
    while(prevVaddr != 0)
    {
        mmap = TranslateVaddr(prevVaddr);
        prevVaddr = ((uint64_t*)((char*)memdev+mmap))[3];
    }
    /* One ELF per file */
    uint8_t* elfDigest = calloc(1, 64);
    CrawlVMAs(name, mmap, pgdPaddr, rodataDigest);
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
    uint64_t parent = TranslateVaddr(parentAddr);
    if(ValidateTaskStruct(parent))
    {
        GetPID(parent, parentPid);
    }
}

void InterpretCred(uint64_t task, struct cred* cred)
{
    uint64_t credAddrLoc = task + 1640 - 8 - 8; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memdev+credAddrLoc))[0];
    uint64_t credPaddr = TranslateVaddr(credVaddr);

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

void InterpretTaskStruct(uint64_t thisTaskStructPaddr, uint64_t* children, uint64_t* sibling)//, uint64_t* parent)
{
    int childLoc = thisTaskStructPaddr + 1232;
    uint64_t firstChild = ((uint64_t*)((char*)memdev+childLoc))[0];
    /* printf("firstChild: %p\n", firstChild); */
    *children = TranslateVaddr(firstChild)-1248;

    int siblingLoc = thisTaskStructPaddr + 1248;
    uint64_t leftSibling = ((uint64_t*)((char*)memdev+siblingLoc))[0];
    /* printf("leftSibling: %p\n", leftSibling); */
    *sibling = TranslateVaddr(leftSibling)-1248;
}

void CollectTaskMeasurement(TaskMeasurement* msmt, uint64_t taskptr)
{
    /* GetTaskName(taskptr, &msmt->name); */
    /* printf("Currently Crawling: %s\n", GetTaskNamePointer(taskptr)); */
    msmt->name = GetTaskNamePointer(taskptr);
    GetPIDs(taskptr, &msmt->myPid, &msmt->parentPid);
    InterpretCred(taskptr, &msmt->cred);

    /* if(strcmp(&msmt->name, "init")==0) */
    {
        bool isKernelThread = msmt->parentPid == 2;
        InterpretMemory(msmt->paddr, &msmt->name, &msmt->rodataDigest);
    }
    return;
}

TaskMeasurement* BuildTaskTreeNode(uint64_t taskPaddr, TaskMeasurement* parent)
{
    TaskMeasurement* thisMsmt = calloc(1,sizeof(TaskMeasurement));
    thisMsmt->paddr = taskPaddr;
    thisMsmt->parent = parent;
    // TODO does this really need to be 64 long?
    thisMsmt->children = calloc(64, sizeof(TaskMeasurement*));
    CollectTaskMeasurement(thisMsmt, taskPaddr);
    // prepare to crawl further
    uint64_t myChild;
    uint64_t mySibling;
    InterpretTaskStruct(taskPaddr, &myChild, &mySibling);
    // if we have any children, enqueue them all
    if(ValidateTaskStruct(myChild))
    {
        int childNumber = 0;
        thisMsmt->children[childNumber++] = BuildTaskTreeNode(myChild, thisMsmt);
        uint64_t dummy;
        uint64_t siblingIterator;
        InterpretTaskStruct(myChild, &dummy, &siblingIterator);
        while( ValidateTaskStruct(siblingIterator) && siblingIterator != myChild )
        {
            thisMsmt->children[childNumber++] = BuildTaskTreeNode(siblingIterator, thisMsmt);
            InterpretTaskStruct(siblingIterator, &dummy, &siblingIterator);
        }
    }
    return thisMsmt;
}

bool AppraiseTaskTree(TaskMeasurement* swapper)
{
    bool appraisalResult = true;
    struct Queue* queue = createQueue(1000);
    enqueue(queue, swapper);
    while(!isEmpty(queue))
    {
        TaskMeasurement* thisTaskMsmt = dequeue(queue);
        if(!IsDigestEmpty(thisTaskMsmt->rodataDigest))
        {
            if(IsThisAKnownDigest(&thisTaskMsmt->rodataDigest))
            {
                printf("Task %s recognized:\n", thisTaskMsmt->name);
            }
            else
            {
                appraisalResult = false;
                printf("Be warned! Task %s NOT recognized:\n", thisTaskMsmt->name);
            }
            RenderDigestDeclaration(thisTaskMsmt->name, thisTaskMsmt->rodataDigest);
        }
        TaskMeasurement** iter = thisTaskMsmt->children;
        while(*iter != NULL)
        {
            enqueue(queue, *iter);
            iter++;
        }
    }
    return appraisalResult;
}

void FreeTaskTree(TaskMeasurement* root)
{
    struct Queue* queue = createQueue(1000);
    enqueue(queue, root);
    while(!isEmpty(queue))
    {
        TaskMeasurement* thisTaskMsmt = dequeue(queue);
        TaskMeasurement** iter = thisTaskMsmt->children;
        while(*iter != NULL)
        {
            enqueue(queue, *iter);
            iter++;
        }
        free(thisTaskMsmt);
    }
}

TaskMeasurement* MeasureTaskTree()
{
    /* printf("DEBUG: Measurement: Beginning task tree measurement.\n"); */
    uint64_t init_task_ptr = (uint64_t)INIT_TASK_ADDR;
    TaskMeasurement* swapperMeasurement = BuildTaskTreeNode(init_task_ptr, NULL);
    swapperMeasurement->parent = swapperMeasurement;
    return swapperMeasurement;
}

