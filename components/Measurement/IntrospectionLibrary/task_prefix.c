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

