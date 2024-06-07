/*
 * Top level functions for linux measurement
 * Include packaging the evidence for transport
 * Michael Neises
 * 13 October 2023
 */

/* 
** Every function here should return a byte pointer that refers to a number of
** bytes that is a multiple of sizeof(EvidenceBundle). That's 64 bytes for a sha512 digest, 8
** bytes for an "evidence type," and 56 bytes for a name string. 56 bytes is
** the longest they get (module names). Task names are only 16 bytes.
*/

/*
** The return pointers should be a list of "structs" like this:
** 8 bytes type | 56 bytes name | 64 bytes digest
*/
#include "EvidenceBundle.h"

const char RODATA_TYPE[8] = {'R','o','d','a','t','a','\0','\0'};
const char MODULE_TYPE[8] = {'M','o','d','u','l','e','\0','\0'};
const char TASK_TYPE[8] = {'T','a','s','k','\0','\0','\0','\0'};

EvidenceBundle* InspectModules(uint8_t* memory_device)
{
    uint8_t (*module_digests)[NUM_MODULE_DIGESTS * DIGEST_NUM_BYTES] = calloc(NUM_MODULE_DIGESTS, DIGEST_NUM_BYTES);
    char (*module_names)[NUM_MODULE_DIGESTS * INTRO_MODULE_NAME_LEN] = calloc(NUM_MODULE_DIGESTS, INTRO_MODULE_NAME_LEN);
    MeasureKernelModules(memory_device, module_digests, module_names);

    EvidenceBundle* evidence = calloc(NUM_MODULE_DIGESTS, sizeof(EvidenceBundle));

    for(int i=0; i<NUM_MODULE_DIGESTS; i++)
    {
        char (*thisModuleName) [INTRO_MODULE_NAME_LEN] = (char (*) [INTRO_MODULE_NAME_LEN])&((*module_names)[i*INTRO_MODULE_NAME_LEN]);
        if(IsThisAValidModuleMeasurement(thisModuleName))
        {
            uint8_t (*thisModuleDigest) [DIGEST_NUM_BYTES] = (uint8_t (*) [DIGEST_NUM_BYTES])&((*module_digests)[i*DIGEST_NUM_BYTES]);
            EvidenceBundle thisBundle = CreateBundle(&MODULE_TYPE, thisModuleName, thisModuleDigest);
            PackBundle(evidence, NUM_MODULE_DIGESTS, &thisBundle, 1);
        }
    }

    free(module_digests);
    free(module_names);

    return evidence;
}

EvidenceBundle* InspectTasks(uint8_t* memory_device)
{
    TaskMeasurement* rootTaskMeasurement = MeasureTaskTree(memory_device);
    FreeTaskTree(rootTaskMeasurement);
    return NULL;
}

EvidenceBundle* InspectRodata(uint8_t* memory_device)
{
    EvidenceBundle* evidence = calloc(1, sizeof(EvidenceBundle));
    uint8_t (*kernelRodataDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureKernelRodata(memory_device, kernelRodataDigest);
    const char rodataBundleName[56] = "KernelRodata";
    EvidenceBundle rodataBundle = CreateBundle(&RODATA_TYPE, &rodataBundleName, kernelRodataDigest);
    PackBundleSingle(evidence, 1, &rodataBundle);
    free(kernelRodataDigest);
    return evidence;
}

EvidenceBundle* InspectSystemCallTable(uint8_t* memory_device)
{
    EvidenceBundle* evidence = calloc(1, sizeof(EvidenceBundle));
    uint8_t (*kernelSCTDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureSystemCallTable(memory_device, kernelSCTDigest);
    const char bundleName[56] = "KernelSystemCallTable";
    EvidenceBundle syscalltableBundle = CreateBundle(&RODATA_TYPE, &bundleName, kernelSCTDigest);
    PackBundleSingle(evidence, 1, &syscalltableBundle);
    free(kernelSCTDigest);
    return evidence;
}

EvidenceBundle* InspectVirtualFileSystems(uint8_t* memory_device)
{
    EvidenceBundle* evidence = calloc(1, sizeof(EvidenceBundle));
    uint8_t (*VFSDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    MeasureFileSystems(memory_device, VFSDigest);
    const char bundleName[56] = "VirtualFileSystems";
    EvidenceBundle vfsBundle = CreateBundle(&RODATA_TYPE, &bundleName, VFSDigest);
    PackBundleSingle(evidence, 1, &vfsBundle);
    free(VFSDigest);
    return evidence;
}

EvidenceBundle* InspectCAmkESFileSystem()
{
    CheckFS();
    return NULL;
}
