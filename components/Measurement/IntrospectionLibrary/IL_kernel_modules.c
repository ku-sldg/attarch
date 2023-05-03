/*
 * An introspection library for Linux 4.9.307
 * Kernel Module Interpretation and Measurement
 * Michael Neises
 * 21 July 2022
 */

struct module_layout {
    uint64_t base;
    unsigned int size;
    unsigned int text_size;
    unsigned int ro_size;
    unsigned int ro_after_init_size;
};

struct module_layout GetModuleLayoutFromListHead(int physAddr)
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
    thisModule.base = ((uint64_t*)((char*)memdev+physAddr+47*8))[0];
    thisModule.size = ((unsigned int*)((char*)memdev+physAddr+47*8))[2];
    thisModule.text_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[3];
    thisModule.ro_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[4];
    thisModule.ro_after_init_size = ((unsigned int*)((char*)memdev+physAddr+47*8))[5];

    return thisModule;
}

void InterpretKernelModule(uint64_t inputAddress, uint8_t (*rodataDigest)[DIGEST_NUM_BYTES], char (*name)[MODULE_NAME_LEN])
{
    bool IKMDebug = false;
    if(IKMDebug)
    {
        printf("Module Address: %016X\n", inputAddress);
    }

    for(int j=16; j<MODULE_NAME_LEN+16; j++)
    {
        (*name)[j-16] = ((char*)memdev)[inputAddress+j];
    }

    char msg[13] = "Found Module ";
    introLog(3, msg, (*name), "\n");

    struct module_layout thisModuleLayout = GetModuleLayoutFromListHead((int)inputAddress);
    uint64_t basePtr = TranslateVaddr(thisModuleLayout.base);

    if(IKMDebug)
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

    int this_module_num_ro_pages = thisModuleLayout.ro_size / PAGE_SIZE;
    uint8_t (*digestArray)[this_module_num_ro_pages * DIGEST_NUM_BYTES] = calloc(this_module_num_ro_pages, DIGEST_NUM_BYTES);
    for(int i=0; i<thisModuleLayout.ro_size / PAGE_SIZE; i++)
    {
        MeasureUserPage((uint8_t*)memdev, (uint8_t (*) [DIGEST_NUM_BYTES])&((*digestArray)[i*DIGEST_NUM_BYTES]), thisModuleLayout.base + i*PAGE_SIZE);
    }
    HashMeasure((uint8_t*)digestArray, this_module_num_ro_pages, rodataDigest);
    free(digestArray);
}

void MeasureKernelModules(uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES], char (*module_names)[NUM_MODULE_DIGESTS * MODULE_NAME_LEN])
{
    bool MKMDebug = false;
    printf("DEBUG: Measurement: Beginning kernel module measurement.\n");
    if(MKMDebug)
    {
        printf("Collecting module pointers...\n");
    }
    /* modulePtrs is a list of offsets into memdev that refer to kernel
    ** modules. They are physical memory addresses with the RAM_BASE
    ** already subtracted.
    */
    uint64_t modulePtrs[NUM_MODULE_DIGESTS];
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        modulePtrs[i] = 0;
    }
    int numModulePtrs = 0;
    uint64_t* list_head_ptr = (uint64_t*)(((char*)memdev)+LIST_HEAD_ADDR);
    uint64_t module_pointer = TranslationTableWalk(list_head_ptr[0]);
    while(module_pointer != LIST_HEAD_ADDR)
    {
        modulePtrs[numModulePtrs] = module_pointer;
        numModulePtrs++;
        char* modBytePtr = ((char*)memdev)+module_pointer;
        uint64_t* modLongPtr = (uint64_t*)modBytePtr;
        module_pointer = TranslationTableWalk(modLongPtr[0]);
    }
    if(MKMDebug)
    {
        printf("Collecting digests over module rodata...\n");
    }
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        if(modulePtrs[i] != 0)
        {
            InterpretKernelModule(modulePtrs[i], (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[DIGEST_NUM_BYTES*i]), (char (*) [MODULE_NAME_LEN])&((*module_names)[MODULE_NAME_LEN*i]));
        }
    }
}
bool IsThisAValidModuleMeasurement(char (*moduleName)[MODULE_NAME_LEN])
{
    for(int i=0; i<MODULE_NAME_LEN; i++)
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

