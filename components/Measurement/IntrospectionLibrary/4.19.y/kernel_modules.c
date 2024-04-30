/*
 * An introspection library for Linux 4.9.307
 * Kernel Module Interpretation and Measurement
 * Michael Neises
 * 21 July 2022
 */

#define MODULE_DEBUG_LOG 0

void ModuleDebugLog(char* msg)
{
    if(MODULE_DEBUG_LOG)
    {
        printf(msg);
    }
}

struct module_layout {
    uint64_t base;
    unsigned int size;
    unsigned int text_size;
    unsigned int ro_size;
    unsigned int ro_after_init_size;
};

struct module_layout GetModuleLayoutFromListHead(uint8_t* memory_device, int physAddr)
{
    int index = physAddr;
    index += 16; // skip list_head
    index += 56; // skip name
    index += 96; //skip mkobj
    index += 8; // skio modinfo_attrs
    index += 8; // skip version
    index += 8; // skip srcversion
    index += 8; // skip holders_dir
    index += 8; // skip syms
    index += 8; // skip crcs
    index += 4; // skip num_syms
    index += 40; // skip struct mutex
    index += 8; // skip kp
    index += 4; // num_kp
    index += 4; // num_gpl_syms
    index += 8; // gpl_syms
    index += 8; // gpl_crcs
    index += 1; //async_probe_requested
    index += 3; // to fill a 4-byte buffer
    index += 8; // gpl_future_syms
    index += 8; // gpl_future_crcs
    index += 4; // num_gpl_future_syms
    index += 4; // num_exentries
    index += 8; // extable
    index += 8; // (*init*(void)

    //by inspection
    struct module_layout thisModule;
    thisModule.base = ((uint64_t*)((char*)memory_device+physAddr+47*8))[0];
    thisModule.size = ((unsigned int*)((char*)memory_device+physAddr+47*8))[2];
    thisModule.text_size = ((unsigned int*)((char*)memory_device+physAddr+47*8))[3];
    thisModule.ro_size = ((unsigned int*)((char*)memory_device+physAddr+47*8))[4];
    thisModule.ro_after_init_size = ((unsigned int*)((char*)memory_device+physAddr+47*8))[5];

    return thisModule;
}

void InterpretKernelModule(uint8_t* memory_device, uint64_t inputAddress, uint8_t (*rodataDigest)[DIGEST_NUM_BYTES], char (*name)[INTRO_MODULE_NAME_LEN])
{
    for(int j=16; j<INTRO_MODULE_NAME_LEN+16; j++)
    {
        (*name)[j-16] = ((char*)memory_device)[inputAddress+j];
    }

    if(MODULE_DEBUG_LOG)
    {
        char msg[13] = "Found Module ";
        introLog(3, msg, (*name), "\n");
    }

    struct module_layout thisModuleLayout = GetModuleLayoutFromListHead(memory_device, (int)inputAddress);
    uint64_t basePtr = TranslateVaddr(memory_device, thisModuleLayout.base);

    if(MODULE_DEBUG_LOG)
    {
        printf("base: %016X\n", thisModuleLayout.base);
        printf("size: %08X\n", thisModuleLayout.size);
        printf("text size: %08X\n", thisModuleLayout.text_size);
        printf("ro size: %08X\n", thisModuleLayout.ro_size);
        /* If a module doesn't have a ro_after_init section, then */
        /* core_layout.ro_after_init_size just takes the value of */
        /* core_layout.ro_size */
        /* https://lwn.net/Articles/695285/ */
        printf("ro after init size: %08X\n", thisModuleLayout.ro_after_init_size);
        printf("base paddr: %016X\n", basePtr);
    }

    HashMeasure(memory_device+basePtr, thisModuleLayout.ro_size, rodataDigest);
}

void MeasureKernelModules(uint8_t* memory_device, uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES], char (*module_names)[NUM_MODULE_DIGESTS * INTRO_MODULE_NAME_LEN])
{
    ModuleDebugLog("DEBUG: Measurement: Beginning kernel module measurement.\n");
    ModuleDebugLog("Collecting module pointers...\n");
    /* modulePtrs is a list of offsets into memory_device that refer to kernel
    ** modules. They are physical memory addresses with the RAM_BASE
    ** already subtracted.
    */
    uint64_t modulePtrs[NUM_MODULE_DIGESTS];
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        modulePtrs[i] = 0;
    }
    int numModulePtrs = 0;
    ModuleDebugLog("Translating modules list_head vaddr\n");
    uint64_t list_head_paddr = TranslateVaddr(memory_device, (uint64_t)INTRO_MODULES_VADDR);
    if(MODULE_DEBUG_LOG)
    {
        printf("Modules list head %llx to %llx\n", INTRO_MODULES_VADDR, list_head_paddr);
    }

    uint64_t* list_head_ptr = (uint64_t*)(((char*)memory_device)+list_head_paddr);
    if(MODULE_DEBUG_LOG)
    {
        printf("Start List Head Contents\n%llx\n%llx\nEnd List Head Contents\n", list_head_ptr[0], list_head_ptr[1]);
    }

    uint64_t module_pointer = TranslateVaddr(memory_device, list_head_ptr[0]);
    while(module_pointer != list_head_paddr)
    {
        modulePtrs[numModulePtrs] = module_pointer;
        numModulePtrs++;
        char* modBytePtr = ((char*)memory_device)+module_pointer;
        uint64_t* modLongPtr = (uint64_t*)modBytePtr;
        module_pointer = TranslateVaddr(memory_device, modLongPtr[0]);
    }
    ModuleDebugLog("Collecting digests over module rodata...\n");
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        if(modulePtrs[i] != 0)
        {
            InterpretKernelModule(memory_device, modulePtrs[i], (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[DIGEST_NUM_BYTES*i]), (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[INTRO_MODULE_NAME_LEN*i]));
        }
    }
}
bool IsThisAValidModuleMeasurement(char (*moduleName)[INTRO_MODULE_NAME_LEN])
{
    for(int i=0; i<INTRO_MODULE_NAME_LEN; i++)
    {
        if((*moduleName)[i] != '\0')
        {
            // an invalid (unused) module name should be completely
            // zeroed out
            return true;
        }
    }
    return false;
}

