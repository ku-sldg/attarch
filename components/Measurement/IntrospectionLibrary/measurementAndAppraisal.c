/*
 * Top level functions for linux measurement
 * Michael Neises
 * 09 June 2023
 */

bool IsModulesOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES] = calloc(NUM_MODULE_DIGESTS, DIGEST_NUM_BYTES);
    char (*module_names)[NUM_MODULE_DIGESTS * INTRO_MODULE_NAME_LEN] = calloc(NUM_MODULE_DIGESTS, INTRO_MODULE_NAME_LEN);
    MeasureKernelModules(memory_device, module_digests, module_names);

    printf("DEBUG: Measurement: Appraising modules\n");
    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        if(IsThisAValidModuleMeasurement((char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN])))
        {
            if(IsThisAKnownDigest((uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES])) )
            {
                printf("Module %s recognized:\n", (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]));
            }
            else
            {
                printf("Be warned! Module %s NOT recognized:\n", (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]));
                result = false;
            }
            //mike
            //RenderDigestDeclaration( &((*module_names)[i*INTRO_MODULE_NAME_LEN]) , &((*module_digests)[i*DIGEST_NUM_BYTES]) );

            //gpt4
            RenderDigestDeclaration( (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]) , (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES]) );

        }
    }
    free(module_digests);
    free(module_names);
    printf("Module Appraisal %s\n", result ? "Passed" : "Failed");
    return result;
}

bool IsTasksOkay(uint8_t* memory_device)
{
    TaskMeasurement* rootTaskMeasurement = MeasureTaskTree(memory_device);
    printf("DEBUG: Measurement: Appraising tasks\n");
    bool result = AppraiseTaskTree(rootTaskMeasurement);
    FreeTaskTree(rootTaskMeasurement);
    printf("Task Appraisal %s\n", result ? "Passed" : "Failed");
    return result;
}

bool IsKernelRodataOkay(uint8_t* memory_device)
{
    bool result = true;
    uint8_t (*kernelRodataDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodata(memory_device, kernelRodataDigest);
    printf("DEBUG: Measurement: Appraising kernel rodata\n");
    if(IsThisAKnownDigest(kernelRodataDigest))
    {
        printf("Kernel Rodata recognized\n");
    }
    else
    {
        printf("Be warned! Kernel Rodata NOT recognized:\n");
        result = false;
    }
    char actualArray[INTRO_MODULE_NAME_LEN] = "KernelRodata";
    char (*rodataName)[INTRO_MODULE_NAME_LEN] = &actualArray;
    RenderDigestDeclaration(rodataName, kernelRodataDigest);
    free(kernelRodataDigest);
    printf("Kernel Rodata Appraisal %s\n", result ? "Passed" : "Failed");
    return result;
}

