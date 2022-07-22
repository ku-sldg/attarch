/*
 * An introspection library for Linux 4.9.307
 * Interpretation and Measurement of Running Tasks
 * Michael Neises
 * 21 July 2022
 */

#include "IL_Struct_Interp.c"

struct cred {
    int usage;
    int uid, gid, suid, sgid, euid, egid, fsuid, fsgid;
    int securebits;
    uint64_t cap_inheritable, cap_permitted, cap_effective, cap_bset, cap_ambient;
    char jit_keyring;
    uint64_t session_keyring, process_keyring, thread_keyring, request_key_auth, security;
    uint64_t user_struct, user_namespace, group_info;
};

struct TaskMeasurement
{
    char name[16];
    int uid;
    uint32_t myPid;
    uint32_t parentPid;
    uint32_t flags;
    struct cred cred;
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
    GetTaskName(task, &results[taskID].name);
    GetPIDs(task, &results[taskID].myPid, &results[taskID].parentPid);
    InterpretCred(task, &results[taskID].cred);

    if(strcmp(&results[taskID].name, "useram")==0)
    {
    }

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
    char suid[10];
    sprintf(suid, "%d", msmt->cred.suid);
    introLog(9, "Task Evidence:\n\tName: ", msmt->name, "\n\tPID: ", &myPid, "\n\tParent PID: ", &parentPid, "\n\tSUID: ", &suid, "\n");
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

