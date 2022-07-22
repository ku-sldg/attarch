/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#include "IL_Struct_Interp.c"

struct TaskMeasurement
{
    char name[16];
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
};

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
    GetTaskName(task, &results[taskID].name);
    GetPIDs(task, &results[taskID].myPid, &results[taskID].parentPid);

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

void PrintTaskEvidence(struct TaskMeasurement* msmt)
{
    char myPid[10];
    sprintf(myPid, "%ld", msmt->myPid);
    char parentPid[10];
    sprintf(parentPid, "%ld", msmt->parentPid);
    introLog(7, "Task Evidence:\n\tName: ", msmt->name, "\n\tPID: ", &myPid, "\n\tParent PID: ", &parentPid, "\n");
}

void MeasureProcesses()
{
    printf("DEBUG: Measurement: Beginning process measurement.\n");

    struct TaskMeasurement* taskMsmts = calloc(100,sizeof(struct TaskMeasurement));
    int numTasksCollected = 0;

    uint64_t init_task_ptr = (uint64_t)INIT_TASK_ADDR;
    CrawlProcesses(init_task_ptr, init_task_ptr, taskMsmts, numTasksCollected);

    for(int i=0; i<100; i++)
    {
        if(taskMsmts[i].name[0] != '\0')
        {
            PrintTaskEvidence(&taskMsmts[i]);
        }
    }

    //ScanTaskStruct(init_task_ptr);
}

