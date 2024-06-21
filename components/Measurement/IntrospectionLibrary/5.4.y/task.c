/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#define INTRO_TASK_DEBUG 0

#include "elf_header.c"

/* C program for array implementation of queue */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

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
    return true;
}


void DebugLog(char* msg)
{
    if(INTRO_TASK_DEBUG)
    {
        printf(msg);
    }
}

/* So we think this cred struct might have some useful bits
** The executor and owner IDs could be collected to ensure
** every task was started and is currently owned in a predictable way.
*/
struct cred {
    int usage;
    int uid, gid, suid, sgid, euid, egid, fsuid, fsgid;
    int securebits;
    uint64_t cap_inheritable, cap_permitted, cap_effective, cap_bset, cap_ambient;
    char jit_keyring;
    uint64_t session_keyring, process_keyring, thread_keyring, request_key_auth, security;
    uint64_t user_struct, user_namespace, group_info;
};

/* This is the form of the output of the introspection work of this file.
** Ultimately we output a tree, where vertices are TaskMeasurements.
*/
typedef struct TaskMeasurement
{
    char name[TASK_COMM_LEN];
    uint8_t rodataDigest[DIGEST_NUM_BYTES];
    uint64_t paddr; // physical address of the task struct in VM memory
    struct TaskMeasurement* parent;
    struct TaskMeasurement* real_parent;
    struct TaskMeasurement* children[NUM_CHILD_TASKS];
    int uid;
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
    struct cred cred;
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
    PrintDigest((uint8_t (*) [DIGEST_NUM_BYTES])(msmt->rodataDigest));
    printf("\n");
}

void GetTaskNamePointer(uint8_t* memory_device, uint64_t task, char (*output_name)[TASK_COMM_LEN])
{
    char* index = ((char*)memory_device) + task + COMM_OFFSET;
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
    int nameLoc = task + COMM_OFFSET;
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
    uint64_t mmAddrLoc = task + MM_OFFSET;
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
    uint64_t pgdVaddr = ((uint64_t*)((char*)memory_device+mm+PGD_OFFSET))[0];
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
    int myPidLoc = task + PID_OFFSET;
    *myPid =((int*)((char*)memory_device+myPidLoc))[0];
}

/* takes an address to a task_struct in vm memory
** returns its pid and its parent's pid
*/
void GetPIDs(uint8_t* memory_device, uint64_t task, int* myPid, int* parentPid)
{
    GetPID(memory_device, task, myPid);
    uint64_t parentAddrLoc = task + PARENT_OFFSET;
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
    uint64_t credAddrLoc = task + REAL_CRED_OFFSET;
    uint64_t credVaddr = ((uint64_t*)((char*)memory_device+credAddrLoc))[0];
    uint64_t credPaddr = TranslateVaddr(memory_device, credVaddr);

    // TODO: these offsets may all have changed
    
    /* cred->usage = ((int*)((char*)memory_device+credPaddr))[0]; */
    /* cred->uid = ((int*)((char*)memory_device+credPaddr+4))[0]; */
    /* cred->gid = ((int*)((char*)memory_device+credPaddr+8))[0]; */
    /* cred->suid = ((int*)((char*)memory_device+credPaddr+12))[0]; */
    /* cred->sgid = ((int*)((char*)memory_device+credPaddr+16))[0]; */
    /* cred->euid = ((int*)((char*)memory_device+credPaddr+20))[0]; */
    /* cred->egid = ((int*)((char*)memory_device+credPaddr+24))[0]; */
    /* cred->fsuid = ((int*)((char*)memory_device+credPaddr+28))[0]; */
    /* cred->fsgid = ((int*)((char*)memory_device+credPaddr+32))[0]; */
    /* cred->securebits = ((int*)((char*)memory_device+credPaddr+36))[0]; */
    /* cred->cap_inheritable = ((uint64_t*)((char*)memory_device+credPaddr+40))[0]; */
    /* cred->cap_permitted = ((uint64_t*)((char*)memory_device+credPaddr+48))[0]; */
    /* cred->cap_effective = ((uint64_t*)((char*)memory_device+credPaddr+56))[0]; */
    /* cred->cap_bset = ((uint64_t*)((char*)memory_device+credPaddr+64))[0]; */
    /* cred->cap_ambient = ((uint64_t*)((char*)memory_device+credPaddr+72))[0]; */
    /* cred->jit_keyring = ((char*)memory_device+credPaddr+80)[0]; */
    /* cred->session_keyring = ((uint64_t*)((char*)memory_device+credPaddr+88))[0]; */
    /* cred->process_keyring = ((uint64_t*)((char*)memory_device+credPaddr+96))[0]; */
    /* cred->thread_keyring = ((uint64_t*)((char*)memory_device+credPaddr+104))[0]; */
    /* cred->request_key_auth = ((uint64_t*)((char*)memory_device+credPaddr+112))[0]; */
    /* cred->security = ((uint64_t*)((char*)memory_device+credPaddr+120))[0]; */
    /* cred->user_struct = ((uint64_t*)((char*)memory_device+credPaddr+128))[0]; */
    /* cred->user_namespace = ((uint64_t*)((char*)memory_device+credPaddr+136))[0]; */
    /* cred->group_info = ((uint64_t*)((char*)memory_device+credPaddr+144))[0]; */

}

/* InterpretTaskStruct takes an address to a task_struct in vm memory
** it returns the addresses of it's leftmost child and left sibling
*/
void InterpretTaskStruct(uint8_t* memory_device, uint64_t thisTaskStructPaddr, uint64_t* children, uint64_t* sibling)//, uint64_t* parent)
{
    int childLoc = thisTaskStructPaddr + CHILDREN_OFFSET;
    uint64_t firstChild = ((uint64_t*)((char*)memory_device+childLoc))[0];
    /* printf("firstChild: %p\n", firstChild); */
    *children = TranslateVaddr(memory_device, firstChild)-SIBLING_OFFSET;

    int siblingLoc = thisTaskStructPaddr + SIBLING_OFFSET;
    uint64_t leftSibling = ((uint64_t*)((char*)memory_device+siblingLoc))[0];
    /* printf("leftSibling: %p\n", leftSibling); */
    *sibling = TranslateVaddr(memory_device, leftSibling)-SIBLING_OFFSET;
}

/* CollectTaskMeasurement takes a TaskMeasurement pointer,
** as well as the address of the related task_struct in vm memory,
** and returns the filled out TaskMeasurement
*/
void CollectTaskMeasurement(uint8_t* memory_device, TaskMeasurement* msmt, uint64_t taskptr)
{
    GetTaskNamePointer(memory_device, taskptr, &msmt->name);
    /* printf("Currently Crawling: %s\n", msmt->name); */
    GetPIDs(memory_device, taskptr, &msmt->myPid, &msmt->parentPid);
    InterpretCred(memory_device, taskptr, &msmt->cred);

    bool isKernelThread = msmt->parentPid == 2;
    DebugLog("before interpret memory\n");
    InterpretMemory(memory_device, msmt->paddr, (uint8_t (*) [DIGEST_NUM_BYTES])(&msmt->rodataDigest));
    DebugLog("after\n");

    return;
}

/* BuildTaskTreeNode takes the address of a task_struct in vm_memory,
** and a pointer to the parent TaskMeasurement,
** and returns a pointer to a newly crafted TaskMeasurement for the input task
*/
TaskMeasurement* BuildTaskTreeNode(uint8_t* memory_device, uint64_t taskPaddr, TaskMeasurement* parent)
{
    TaskMeasurement* thisMsmt = calloc(1,sizeof(TaskMeasurement));
    thisMsmt->paddr = taskPaddr;
    thisMsmt->parent = parent;
    DebugLog("before collect measurement\n");
    CollectTaskMeasurement(memory_device, thisMsmt, taskPaddr);
    /* PrintTaskEvidence(thisMsmt); */
    DebugLog("after\n");
    // prepare to crawl further
    uint64_t myChild;
    uint64_t mySibling;
    DebugLog("interpret task struct\n");
    InterpretTaskStruct(memory_device, taskPaddr, &myChild, &mySibling);
    DebugLog("after\n");
    // if we have any children, enqueue them all
    if(ValidateTaskStruct(memory_device, myChild))
    {
        int childNumber = 0;
        thisMsmt->children[childNumber++] = BuildTaskTreeNode(memory_device, myChild, thisMsmt);
        /* PrintTaskEvidence(thisMsmt->children[childNumber-1]); */
        uint64_t dummy;
        uint64_t siblingIterator;
        InterpretTaskStruct(memory_device, myChild, &dummy, &siblingIterator);
        while( ValidateTaskStruct(memory_device, siblingIterator) && siblingIterator != myChild )
        {
            thisMsmt->children[childNumber++] = BuildTaskTreeNode(memory_device, siblingIterator, thisMsmt);
            InterpretTaskStruct(memory_device, siblingIterator, &dummy, &siblingIterator);
        }
    }
    return thisMsmt;
}

/* AppraiseTaskTree takes the root TaskMeasurement of a measured task tree
** and returns the result of appraising the tree.
** In particular, we execute a breadth-first search.
*/
bool AppraiseTaskTree(TaskMeasurement* swapper)
{
    bool appraisalResult = true;
    struct Queue* queue = createQueue(1000);
    enqueue(queue, swapper);
    while(!isEmpty(queue))
    {
        TaskMeasurement* thisTaskMsmt = dequeue(queue);
        if(!IsDigestEmpty((uint8_t (*)[DIGEST_NUM_BYTES])(thisTaskMsmt->rodataDigest)))
        {
            if(IsThisAKnownDigest((uint8_t (*)[DIGEST_NUM_BYTES])thisTaskMsmt->rodataDigest))
            {
                printf("Task %s recognized:\n", thisTaskMsmt->name);
            }
            else
            {
                appraisalResult = false;
                printf("Be warned! Task %s NOT recognized:\n", thisTaskMsmt->name);
            }
            RenderDigestDeclaration((char (*) [INTRO_MODULE_NAME_LEN])(thisTaskMsmt->name), (uint8_t (*)[DIGEST_NUM_BYTES])thisTaskMsmt->rodataDigest);
        }
        for(int i=0; i<NUM_CHILD_TASKS; i++)
        {
            TaskMeasurement* iter = thisTaskMsmt->children[i];
            if(iter != NULL)
            {
                enqueue(queue, iter);
                continue;
            }
            break;
        }
    }
    free(queue->array);
    free(queue);
    return appraisalResult;
}

/* Clean up our memory
*/
void FreeTaskTree(TaskMeasurement* root)
{
    struct Queue* queue = createQueue(1000);
    enqueue(queue, root);
    while(!isEmpty(queue))
    {
        TaskMeasurement* thisTaskMsmt = dequeue(queue);
        for(int i=0; i<NUM_CHILD_TASKS; i++)
        {
            TaskMeasurement* iter = thisTaskMsmt->children[i];
            if(iter != NULL)
            {
                enqueue(queue, iter);
                continue;
            }
            break;
        }
        free(thisTaskMsmt);
    }
    free(queue->array);
    free(queue);
}

void DigestTaskTree(TaskMeasurement* root, uint8_t (*digest)[DIGEST_NUM_BYTES])
{
    struct Queue* queue = createQueue(1000);
    enqueue(queue, root);
    while(!isEmpty(queue))
    {
        TaskMeasurement* thisTaskMsmt = dequeue(queue);
        if(!IsDigestEmpty((uint8_t (*)[DIGEST_NUM_BYTES])(thisTaskMsmt->rodataDigest)))
        {
            RenderDigestDeclaration((char (*) [INTRO_MODULE_NAME_LEN])(thisTaskMsmt->name), (uint8_t (*)[DIGEST_NUM_BYTES])thisTaskMsmt->rodataDigest);
        }
        for(int i=0; i<NUM_CHILD_TASKS; i++)
        {
            TaskMeasurement* iter = thisTaskMsmt->children[i];
            if(iter != NULL)
            {
                enqueue(queue, iter);
                continue;
            }
            break;
        }
        uint8_t (*tempDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);

        int staticPrefixLen = TASK_COMM_LEN+DIGEST_NUM_BYTES;
        HashMeasure((char*)thisTaskMsmt, staticPrefixLen, tempDigest);
        HashExtend(digest, tempDigest); // extend hash by a zero string
        free(tempDigest);
    }
    free(queue->array);
    free(queue);
    return;
}

/* MeasureTaskTree returns a tree of TaskMeasurements which correspond
** to the real task tree in VM memory. It requires a defined value:
** INTRO_INIT_TASK_VADDR, which is collected from the System.map file generated as a
** side-effect of linux kernel compilation.
*/
TaskMeasurement* MeasureTaskTree(uint8_t* memory_device)
{
    DebugLog("Measurement: Beginning task tree measurement.\n");
    uint64_t init_task_paddr = TranslateVaddr(memory_device, (uint64_t)INTRO_INIT_TASK_VADDR);
    DebugLog("before build tree\n");
    TaskMeasurement* swapperMeasurement = BuildTaskTreeNode(memory_device, init_task_paddr, NULL);
    DebugLog("after build tree\n");
    swapperMeasurement->parent = swapperMeasurement;
    return swapperMeasurement;
}

