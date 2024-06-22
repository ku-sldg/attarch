/*
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * June 2024
 */

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
        HashMeasure((uint8_t*)thisTaskMsmt, 0, staticPrefixLen, tempDigest);
        HashExtend(digest, tempDigest); // extend hash by a zero string
        free(tempDigest);
    }
    free(queue->array);
    free(queue);
    return;
}

/* BuildTaskTree returns a tree of TaskMeasurements which correspond
** to the real task tree in VM memory. It requires a defined value:
** INTRO_INIT_TASK_VADDR, which is collected from the System.map file generated as a
** side-effect of linux kernel compilation.
*/
TaskMeasurement* BuildTaskTree(uint8_t* memory_device)
{
    DebugLog("Measurement: Beginning task tree measurement.\n");
    uint64_t init_task_paddr = TranslateVaddr(memory_device, (uint64_t)INTRO_INIT_TASK_VADDR);
    DebugLog("before build tree\n");
    TaskMeasurement* swapperMeasurement = BuildTaskTreeNode(memory_device, init_task_paddr, NULL);
    DebugLog("after build tree\n");
    swapperMeasurement->parent = swapperMeasurement;
    return swapperMeasurement;
}

EvidenceBundle* MeasureTaskTree(uint8_t* memory_device, int* count) 
{
    const char name[56] = "ProcessTree";
    uint8_t (*digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    if(CanMeasureTasks())
    {
        TaskMeasurement* msmt = BuildTaskTree(memory_device);
        DigestTaskTree(msmt, digest);
        FreeTaskTree(msmt);
    }
    EvidenceBundle* bundle = AllocBundle(&TASK_TYPE, name, digest);
    free(digest);
    *count = 1;
    return bundle;
}


