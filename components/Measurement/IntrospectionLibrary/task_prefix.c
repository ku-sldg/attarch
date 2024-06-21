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
    uint64_t credAddrLoc = task + REAL_CRED_OFFSET; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memory_device+credAddrLoc))[0];
    uint64_t credPaddr = TranslateVaddr(memory_device, credVaddr);
    // These offsets may all have changed
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
    if(!ValidateTaskStruct(memory_device, *sibling) )
    {
        // fall back to a table walk
        *sibling = TranslationTableWalk(memory_device, leftSibling)-SIBLING_OFFSET;
    }
}

