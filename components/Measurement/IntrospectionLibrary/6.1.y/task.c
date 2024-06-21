#include "maple_tree.c"

bool CanMeasureTasks()
{
    return true;
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
    uint64_t mmAddr = ((uint64_t*)((char*)memory_device+taskPaddr+1208))[0];
    if(mmAddr==0)
    {
        mmAddr = ((uint64_t*)((char*)memory_device+taskPaddr+1208))[1];
        if(mmAddr==0)
        {
            return 0;
        }
    }
    uint64_t mmPaddr = TranslateVaddr(memory_device, mmAddr);
    uint64_t pgdVaddr = ((uint64_t*)((char*)memory_device+mmPaddr+56))[0];
    uint64_t pgdPaddr = TranslateVaddr(memory_device, pgdVaddr);
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
    uint64_t mmPaddr = TranslateVaddr(memory_device, mmVaddr);
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
    DebugLog("Interpret Cred\n");
    uint64_t credAddrLoc = task + 1832; //"real_cred"
    uint64_t credVaddr = ((uint64_t*)((char*)memory_device+credAddrLoc))[0];
    uint64_t credPaddr = TranslateVaddr(memory_device, credVaddr);

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
    *children = TranslateVaddr(memory_device, firstChild)-1400; //the offset of siblings list_head in task_struct

    int siblingLoc = thisTaskStructPaddr + 1400; //the offset of sibling in task_struct
    uint64_t leftSibling = ((uint64_t*)((char*)memory_device+siblingLoc))[0];
    /* DebugLog("leftSibling: %p\n", leftSibling); */
    *sibling = TranslateVaddr(memory_device, leftSibling)-1400; //the offset of siblings list_head in task_struct
    if( !ValidateTaskStruct(memory_device, *sibling) )
    {
        // fall back to a table walk here.
        *sibling = TranslationTableWalk(memory_device, leftSibling)-1400; //the offset of sibling in task_struct
    }
}

