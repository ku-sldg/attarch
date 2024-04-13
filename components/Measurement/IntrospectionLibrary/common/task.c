/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#define INTRO_TASK_DEBUG 0

#include "struct_interp.c"
#include "elf_header.c"
#include "maple_tree.c"

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
** TODO
** NOTE: in the Linux Standard Base Core - Generic 5.0,
** UIDs 0-99 are reserved for the system.
** This and other specifications can be found in the LSB, presumably
** https://refspecs.linuxfoundation.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic.pdf
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
    uint64_t paddr; // physical address of the task struct in VM memory
    struct TaskMeasurement* parent;
    struct TaskMeasurement* real_parent;
    struct TaskMeasurement* children[NUM_CHILD_TASKS];
    char name[TASK_COMM_LEN];
    int uid;
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
    struct cred cred;
    uint8_t rodataDigest[DIGEST_NUM_BYTES];
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
    printf("Task Evidence:\n\tName: %s\n\tPID: %s\n\tParent PID: %s\n\tSUID: %s\n\t Read-only Data Digest:\n", msmt->name, &myPid, &parentPid, &suid);
    PrintDigest((uint8_t (*) [DIGEST_NUM_BYTES])(msmt->rodataDigest));
    printf("\n");
}

void GetTaskNamePointer(uint8_t* memory_device, uint64_t task, char (*output_name)[TASK_COMM_LEN])
{
    char* index = ((char*)memory_device) + task + 1856;
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
    int nameLoc = task + 1856;
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
            return false;
        }
    }
    return true;
}

uint64_t GetPgdPaddrFromTask(uint8_t* memory_device, uint64_t taskPaddr)
{
    DebugLog("GetPgdPaddrFromTask\n");
    uint64_t mmAddr = ((uint64_t*)((char*)memory_device+taskPaddr+1208))[0];
    if(mmAddr==0)
    {
        mmAddr = ((uint64_t*)((char*)memory_device+taskPaddr+1208))[1];
        if(mmAddr==0)
        {
            return 0;
        }
    }
    uint64_t mmPaddr = intro_virt_to_phys(mmAddr);
    uint64_t pgdVaddr = ((uint64_t*)((char*)memory_device+mmPaddr+56))[0];
    DebugLog("GetPgdPaddrFromTask part 2\n");
    uint64_t pgdPaddr = intro_virt_to_phys(pgdVaddr);
    return pgdPaddr;
}

/* InterpretMemory takes a pointer to a valid task_struct in VM memory "task"
** and returns a hash digest of its rodata (if it exists)
**
** We retrieve the pointer to a maple tree from within the task's mm_struct.
** In task, at offset 1208, we find the mm pointer. VALIDATED
** In mm, at offset 0, we find the maple tree struct (NOT A POINTER). VALIDATED
** in the mt struct, at offset 8, we find the maple tree root node. VALIDATED
** In each maple node, at offset 36, we find the maple_type. 
*/
void InterpretMemory(uint8_t* memory_device, uint64_t task, uint8_t (*rodataDigest)[DIGEST_NUM_BYTES])
{
    DebugLog("InterpretMemory\n");
    uint64_t pgdPaddr = GetPgdPaddrFromTask(memory_device, task);
    uint64_t mmVaddr = ((uint64_t*)((char*)memory_device+task+1208))[0];
    if(mmVaddr==0) // If there is no "mm," then check for an "active_mm."
    {
        mmVaddr = ((uint64_t*)((char*)memory_device+task+1208))[1];
        if(mmVaddr==0)
        {
            return; // if there's no mm at all...
        }
    }
    uint64_t mmPaddr = intro_virt_to_phys(mmVaddr);
    uint64_t maple_tree_struct_paddr = mmPaddr;
    StartDumpProcedure(memory_device, maple_tree_struct_paddr, pgdPaddr, rodataDigest);
}

/* takes an address to a task_struct in vm memory
** returns its pid
*/
void GetPID(uint8_t* memory_device, uint64_t task, int* myPid)
{
    int myPidLoc = task + 1352;
    *myPid =((int*)((char*)memory_device+myPidLoc))[0];
}

/* takes an address to a task_struct in vm memory
** returns its pid and its parent's pid
*/
void GetPIDs(uint8_t* memory_device, uint64_t task, int* myPid, int* parentPid)
{
    DebugLog("Get PIDs\n");
    GetPID(memory_device, task, myPid);
    uint64_t parentAddrLoc = task + 1368; //the offset of real_parent in task_struct
    uint64_t parentAddr = ((uint64_t*)((char*)memory_device+parentAddrLoc))[0];
    uint64_t parent = intro_virt_to_phys(parentAddr);
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
    DebugLog("Interpret Cred\n");
    uint64_t credAddrLoc = task + 1832; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memory_device+credAddrLoc))[0];
    uint64_t credPaddr = intro_virt_to_phys(credVaddr);

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
    DebugLog("Interpret Task Struct\n");
    int childLoc = thisTaskStructPaddr + 1384; //the offset of children in task_struct
    uint64_t firstChild = ((uint64_t*)((char*)memory_device+childLoc))[0];
    /* DebugLog("firstChild: %p\n", firstChild); */
    *children = intro_virt_to_phys(firstChild)-1400; //the offset of siblings list_head in task_struct

    DebugLog("Interpret Task Struct part 2\n");
    int siblingLoc = thisTaskStructPaddr + 1400; //the offset of sibling in task_struct
    uint64_t leftSibling = ((uint64_t*)((char*)memory_device+siblingLoc))[0];
    /* DebugLog("leftSibling: %p\n", leftSibling); */
    *sibling = intro_virt_to_phys(leftSibling)-1400; //the offset of siblings list_head in task_struct
    if( !ValidateTaskStruct(memory_device, *sibling) )
    {
        // fall back to a table walk here.
        /* printf("fallback task\n"); */
        *sibling = TranslationTableWalk(memory_device, leftSibling)-1400; //the offset of sibling in task_struct
    }
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
    DebugLog("after\n");
    // prepare to crawl further
    uint64_t myChild;
    uint64_t mySibling;
    DebugLog("interpret task struct\n");
    InterpretTaskStruct(memory_device, taskPaddr, &myChild, &mySibling);
    DebugLog("after\n");
    // if we have any children, enqueue them all
    /* printf("==========check children==========\n"); */
    if(ValidateTaskStruct(memory_device, myChild))
    {
        int childNumber = 0;

        /* printf("==========iterate siblings=========\n"); */
        uint64_t dummy;
        uint64_t siblingIterator;
        InterpretTaskStruct(memory_device, myChild, &dummy, &siblingIterator);
        while( ValidateTaskStruct(memory_device, siblingIterator) && siblingIterator != myChild )
        {
            thisMsmt->children[childNumber++] = BuildTaskTreeNode(memory_device, siblingIterator, thisMsmt);
            InterpretTaskStruct(memory_device, siblingIterator, &dummy, &siblingIterator);
        }
        /* printf("==========done with siblings=========\n"); */

        thisMsmt->children[childNumber++] = BuildTaskTreeNode(memory_device, myChild, thisMsmt);
    }
    /* printf("node built\n"); */
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
        /* PrintTaskEvidence(thisTaskMsmt); */
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

/* BuildTaskTreeNode takes the address of a task_struct in vm_memory,
** and a pointer to the parent TaskMeasurement,
** and returns a pointer to a newly crafted TaskMeasurement for the input task
*/
TaskMeasurement* IterateSchedulingQueue(uint8_t* memory_device, uint64_t taskPaddr, TaskMeasurement* parent)
{
    // get swapper task_struct
    

    // get sched_entity se from swapper task_struct
    

    // use list_head group_node from sched_entity se to iterate through all
    // scheduled "normal" tasks (not real-time tasks)
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

/* MeasureTaskTree returns a tree of TaskMeasurements which correspond
** to the real task tree in VM memory. It requires a defined value:
** INTRO_INIT_TASK_VADDR, which is collected from the System.map file generated as a
** side-effect of linux kernel compilation.
*/
TaskMeasurement* MeasureTaskTree(uint8_t* memory_device)
{
    DebugLog("Measurement: Beginning task tree measurement.\n");
    uint64_t init_task_paddr = sysmap_virt_to_phys((uint64_t)INTRO_INIT_TASK_VADDR);
    DebugLog("before build tree\n");
    TaskMeasurement* swapperMeasurement = BuildTaskTreeNode(memory_device, init_task_paddr, NULL);
    /* TaskMeasurement* swapperMeasurement = IterateSchedulingQueue(memory_device, init_task_paddr, NULL); */
    /* TaskMeasurement* swapperMeasurement = IterateRealtimeSchedulingQueue(memory_device, init_task_paddr, NULL); */
    DebugLog("after build tree\n");
    swapperMeasurement->parent = swapperMeasurement;

    AppraiseTaskTree(swapperMeasurement);

    return swapperMeasurement;
}

