/*
 * Top level functions for linux measurement
 * Include packaging the evidence for transport
 * Michael Neises
 * 13 October 2023
 */

/* 
** Every function here should return a byte pointer that refers to a number of
** bytes that is a multiple of EVIDENCE_ENTRY_SIZE. That's 64 bytes for a sha512 digest, 8
** bytes for an "evidence type," and 56 bytes for a name string. 56 bytes is
** the longest they get (module names). Task names are only 16 bytes.
*/

/*
** The return pointers should be a list of "structs" like this:
** 8 bytes type | 56 bytes name | 64 bytes digest
*/
#define EVIDENCE_ENTRY_SIZE 128
#define EVIDENCE_TYPE_SIZE 8
#define EVIDENCE_NAME_SIZE (INTRO_MODULE_NAME_LEN < TASK_COMM_LEN) ? TASK_COMM_LEN : INTRO_MODULE_NAME_LEN

/*
** The evidence type should simply refer to module/task/rodata measurements
*/


char RODATA_TYPE[] = {'R','o','d','a','t','a','\0','\0'};
char MODULE_TYPE[] = {'M','o','d','u','l','e','\0','\0'};
char TASK_TYPE[] = {'T','a','s','k','\0','\0','\0','\0'};

void RenderEvidenceWad(char* type, char (*name) [INTRO_MODULE_NAME_LEN], uint8_t (*digest) [DIGEST_NUM_BYTES], uint8_t* destination)
{
    for(int i=0; i<8; i++)
    {
        destination[i] = type[i];
    }
    for(int i=8; i<64; i++)
    {
        destination[i] = (*name)[i-8];
    }
    for(int i=64; i<128; i++)
    {
        destination[i] = (*digest)[i-64];
    }
}

uint8_t* InspectModules(uint8_t* memory_device)
{
    uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES] = calloc(NUM_MODULE_DIGESTS, DIGEST_NUM_BYTES);
    char (*module_names)[NUM_MODULE_DIGESTS * INTRO_MODULE_NAME_LEN] = calloc(NUM_MODULE_DIGESTS, INTRO_MODULE_NAME_LEN);
    MeasureKernelModules(memory_device, module_digests, module_names);

    uint8_t* evidence = calloc(NUM_MODULE_DIGESTS, EVIDENCE_ENTRY_SIZE);

    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        char (*thisModuleName) [INTRO_MODULE_NAME_LEN] = (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]);
        if(IsThisAValidModuleMeasurement(thisModuleName))
        {
            uint8_t (*thisModuleDigest) [DIGEST_NUM_BYTES] = (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES]);
            PrintDigest(thisModuleDigest);
            RenderEvidenceWad(MODULE_TYPE, thisModuleName, thisModuleDigest, evidence+(EVIDENCE_ENTRY_SIZE*i));
        }
    }

    free(module_digests);
    free(module_names);

    return evidence;
}

uint8_t* InspectTasks(uint8_t* memory_device)
{
    TaskMeasurement* rootTaskMeasurement = MeasureTaskTree(memory_device);
    FreeTaskTree(rootTaskMeasurement);
    return NULL;
}

uint8_t* InspectRodata(uint8_t* memory_device)
{
    uint8_t (*kernelRodataDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodata(memory_device, kernelRodataDigest);

    uint8_t* unitCollection = calloc(1,EVIDENCE_ENTRY_SIZE);
    RenderEvidenceWad(RODATA_TYPE, "Kernel Rodata", kernelRodataDigest, unitCollection);

    free(kernelRodataDigest);
    return unitCollection;
}

