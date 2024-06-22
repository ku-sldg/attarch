/*
 * An introspection library for Linux 4.9, 4.14, 4.19, 5.4, 5.10, 5.15, and 6.1
 * You must choose the correct x.y.z/shelf.c for the version of linux.
 * Michael Neises
 * April 2024
 */

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define INCLUDE_SHELF(version) TOSTRING(version/shelf.c)
#define INCLUDE_DEFS(version) TOSTRING(version/definitions.c)
#define BAD_ADDRESS_EXIT_STR "Bad Address. Check #define LINUX_VERSION in introspect_component.c, which must match the version of Linux being used.\n"

#include "armv8a.h"
#include "EvidenceBundle.h"

typedef EvidenceBundle* (*MeasurementRoutine)(uint8_t* memory_device, int* count);

const char RODATA_TYPE[8] = {'R','o','d','a','t','a','\0','\0'};
const char MODULE_TYPE[8] = {'M','o','d','u','l','e','\0','\0'};
const char TASK_TYPE[8] = {'T','a','s','k','\0','\0','\0','\0'};
const char FILE_SYSTEM_TYPE[8] = {'F','i','l','e','S','y','s','.'};

EvidenceBundle* RunMeasurement(uint8_t* memory_device, MeasurementRoutine measure, int* count)
{
    return measure(memory_device, count);
}

#include "hash.h"
#include "KnownDigests.h"
#include INCLUDE_DEFS(LINUX_VERSION)
#include "AddressTranslation.c"
#include "task_prefix.c"
#include INCLUDE_SHELF(LINUX_VERSION)
#include "task_postfix.c"
#include "modules_postfix.c"
#include "kernel_rodata_postfix.c"
#include "filesystems_postfix.c"
#include "system_call_table.c"
#include "measurementAndAppraisal.c"
#include "CAmkESFS.c"

/* This function is here to easily control which measurements */
/* are performed for which versions of linux. */
bool MeasureAndAppraiseLinux(uint8_t* memory_device)
{
        bool rodata_appraisal = IsKernelRodataOkay((uint8_t*)memory_device);
        bool modules_appraisal = IsModulesOkay((uint8_t*)memory_device);
        bool tasks_appraisal =  IsTasksOkay((uint8_t*)memory_device);
        bool overall_appraisal = true;
        overall_appraisal &= rodata_appraisal;
        overall_appraisal &= modules_appraisal;
        overall_appraisal &= tasks_appraisal;
        return overall_appraisal;
}

void ExtendMeasurementList(uint8_t* memory_device, EvidenceBundle** master, MeasurementRoutine measurement)
{
    int count = 0;
    EvidenceBundle* evidence = RunMeasurement(memory_device, measurement, &count);
    for(int i=0; i<count; i++)
    {
        AppendBundle(master, evidence[i]);
    }
}

EvidenceBundle* MeasureLinuxKernel(uint8_t* memory_device)
{
    EvidenceBundle* master = NULL;
    ExtendMeasurementList(memory_device, &master, MeasureKernelRodata);
    ExtendMeasurementList(memory_device, &master, MeasureTaskTree);
    ExtendMeasurementList(memory_device, &master, MeasureFileSystems);
    ExtendMeasurementList(memory_device, &master, MeasureSystemCallTable);
    ExtendMeasurementList(memory_device, &master, MeasureKernelModules);
    ExtendMeasurementList(memory_device, &master, MeasureCAmkESInitRD);
    return master;
}

bool AppraiseLinuxKernelMeasurement(const char* evidence)
{
    EvidenceBundle* bundle = (EvidenceBundle*)(evidence);
    bool result = true;
    while(!IsFinalBundle(*bundle))
    {
        PrintBundle(bundle);
        if(IsThisAKnownDigest(&(bundle->digest)))
        {
            printf("Digest Recognized.\n");
        }
        else
        {
            printf("Digest NOT Recognized:\n");
            RenderDigestDeclaration(&(bundle->name), &(bundle->digest));
            result = false;
        }
        printf("\n");
        bundle++;
    }
    return result;
}

