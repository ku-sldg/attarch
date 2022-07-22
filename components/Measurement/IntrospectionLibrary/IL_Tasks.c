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

