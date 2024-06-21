#include "maple_tree.c"

#define MM_OFFSET 1208
#define PID_OFFSET 1352
#define PARENT_OFFSET 1368
#define CHILDREN_OFFSET 1384
#define SIBLING_OFFSET 1400
#define REAL_CRED_OFFSET 1832
#define COMM_OFFSET 1856
#define PGD_OFFSET 56

bool CanMeasureTasks()
{
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
    uint64_t pgdVaddr = ((uint64_t*)((char*)memory_device+mmPaddr+PGD_OFFSET))[0];
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
    uint64_t mmVaddr = ((uint64_t*)((char*)memory_device+task+MM_OFFSET))[0];
    if(mmVaddr==0) // If there is no "mm," then check for an "active_mm."
    {
        mmVaddr = ((uint64_t*)((char*)memory_device+task+MM_OFFSET))[1];
        if(mmVaddr==0)
        {
            return; // if there's no mm at all...
        }
    }
    uint64_t mmPaddr = TranslateVaddr(memory_device, mmVaddr);
    uint64_t maple_tree_struct_paddr = mmPaddr;
    StartDumpProcedure(memory_device, maple_tree_struct_paddr, pgdPaddr, rodataDigest);
}

/* InterpretTaskStruct takes an address to a task_struct in vm memory
** it returns the addresses of it's leftmost child and left sibling
*/
void InterpretTaskStruct(uint8_t* memory_device, uint64_t thisTaskStructPaddr, uint64_t* children, uint64_t* sibling)//, uint64_t* parent)
{
    int childLoc = thisTaskStructPaddr + CHILDREN_OFFSET; //the offset of children in task_struct
    uint64_t firstChild = ((uint64_t*)((char*)memory_device+childLoc))[0];
    /* DebugLog("firstChild: %p\n", firstChild); */
    *children = TranslateVaddr(memory_device, firstChild)-SIBLING_OFFSET; //the offset of siblings list_head in task_struct

    int siblingLoc = thisTaskStructPaddr + SIBLING_OFFSET; //the offset of sibling in task_struct
    uint64_t leftSibling = ((uint64_t*)((char*)memory_device+siblingLoc))[0];
    /* DebugLog("leftSibling: %p\n", leftSibling); */
    *sibling = TranslateVaddr(memory_device, leftSibling)-SIBLING_OFFSET; //the offset of siblings list_head in task_struct
    if( !ValidateTaskStruct(memory_device, *sibling) )
    {
        // fall back to a table walk here.
        *sibling = TranslationTableWalk(memory_device, leftSibling)-SIBLING_OFFSET; //the offset of sibling in task_struct
    }
}

