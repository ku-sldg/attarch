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

typedef void (*MeasurementRoutine)(uint8_t* memory_device, uint8_t (*digest)[DIGEST_NUM_BYTES]);


EvidenceBundle RunMeasurement(uint8_t* memory_device, const char name[56], MeasurementRoutine measure)
{
    uint8_t (*digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    measure(memory_device, digest);
    EvidenceBundle bundle = CreateBundle(&RODATA_TYPE, name, digest);
    free(digest);
    return bundle;
}

EvidenceBundle* PrepareLoneBundle(EvidenceBundle bundle)
{
    EvidenceBundle* evidence = calloc(1, sizeof(EvidenceBundle));
    PackBundleSingle(evidence, 1, &bundle);
    return evidence;
}

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
    const char bundleName[56] = "ProcessTree";
    EvidenceBundle bundle = RunMeasurement(memory_device, bundleName, MeasureTaskTree);
    return PrepareLoneBundle(bundle);
}

EvidenceBundle* InspectRodata(uint8_t* memory_device)
{
    const char bundleName[56] = "KernelRodata";
    EvidenceBundle bundle = RunMeasurement(memory_device, bundleName, MeasureKernelRodata);
    return PrepareLoneBundle(bundle);
}

EvidenceBundle* InspectSystemCallTable(uint8_t* memory_device)
{
    const char bundleName[56] = "KernelSystemCallTable";
    EvidenceBundle bundle = RunMeasurement(memory_device, bundleName, MeasureSystemCallTable);
    return PrepareLoneBundle(bundle);
}

EvidenceBundle* InspectVirtualFileSystems(uint8_t* memory_device)
{
    const char bundleName[56] = "VirtualFileSystems";
    EvidenceBundle bundle = RunMeasurement(memory_device, bundleName, MeasureFileSystems);
    return PrepareLoneBundle(bundle);
}

EvidenceBundle* InspectCAmkESFileSystem()
{
    CheckFS();
    return NULL;
}
