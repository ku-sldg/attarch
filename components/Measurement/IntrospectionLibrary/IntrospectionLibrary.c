/*
 * An introspection library for Linux 4.9, 4.14, 4.19, 5.4, 5.10, 5.15, and 6.1
 * You must choose the correct x.y.z/shelf.c for the version of linux.
 * Michael Neises
 * April 2024
 */

#include "armv8a.h"
#include "EvidenceBundle.h"
#include "hash.h"
#include "KnownDigests.h"
#include "logging.c"
#include "CHOOSE/shelf.c" // choose your version of linux
#include "system_call_table.c"
#include "measurementAndAppraisal.c"
#include "CAmkESFS.c"
#include "measurements.c"

/* This function is here to easily control which measurements */
/* are performed for which versions of linux. */
bool MeasureAndAppraiseLinux()
{
        bool rodata_appraisal = IsKernelRodataOkay((uint8_t*)memdev);
        bool modules_appraisal = IsModulesOkay((uint8_t*)memdev);
        bool tasks_appraisal =  IsTasksOkay((uint8_t*)memdev);

        bool overall_appraisal = true;
        overall_appraisal &= rodata_appraisal;
        overall_appraisal &= modules_appraisal;
        overall_appraisal &= tasks_appraisal;
       
        return overall_appraisal;
}

EvidenceBundle* MeasureLinuxKernel()
{
    int numRodataEvidences = 0;
    int numModuleEvidences = 0;
    int numTaskEvidences = 0;

    // Execute the measurements
    // we have to free these
    EvidenceBundle* rodataEvidence = InspectRodata((uint8_t*)memdev);
    EvidenceBundle* modulesEvidence = InspectModules((uint8_t*)memdev);
    EvidenceBundle* tasksEvidence = InspectTasks((uint8_t*)memdev);
    EvidenceBundle* sctEvidence = InspectSystemCallTable((uint8_t*)memdev);
    EvidenceBundle* vfsEvidence = InspectVirtualFileSystems((uint8_t*)memdev);

    EvidenceBundle* cfsEvidence =  InspectCAmkESFileSystem();

    /* printf("Here is rodata evidence:\n"); */
    /* PrintBundle(rodataEvidence); */

    // Count the evidence entries so we know how much space we need.
    int numEvidenceEntries = 0;
    numEvidenceEntries++; // we want to null terminate this list
    numEvidenceEntries++; // one for the kernel ro-data
    numEvidenceEntries++; // one for the system call table
    numEvidenceEntries++; // one for the VFS layer

    numModuleEvidences += GetCollectionLength(modulesEvidence, NUM_MODULE_DIGESTS * sizeof(EvidenceBundle));
    numEvidenceEntries += numModuleEvidences;

    numTaskEvidences += GetCollectionLength(tasksEvidence, NUM_TASKS * sizeof(EvidenceBundle));
    numEvidenceEntries += numTaskEvidences;
    
    // allocate memory
    EvidenceBundle* evidenceCollection = calloc(numEvidenceEntries, sizeof(EvidenceBundle));

    // pack the evidence into a single collection
    PackBundleSingle(evidenceCollection, numEvidenceEntries, rodataEvidence);
    PackBundleSingle(evidenceCollection, numEvidenceEntries, sctEvidence);
    PackBundleSingle(evidenceCollection, numEvidenceEntries, vfsEvidence);
    PackBundle(evidenceCollection, numEvidenceEntries, modulesEvidence, numModuleEvidences);
    PackBundle(evidenceCollection, numEvidenceEntries, tasksEvidence, numTaskEvidences);
    PackBundleSingle(evidenceCollection, numEvidenceEntries, &nullEvidenceBundle); // null-terminate the list

    // free the extra bundles
    if(rodataEvidence != NULL)
    {
        free(rodataEvidence);
    }
    if(modulesEvidence != NULL)
    {
        free(modulesEvidence);
    }
    if(tasksEvidence != NULL)
    {
        free(tasksEvidence);
    }
    if(sctEvidence != NULL)
    {
        free(sctEvidence);
    }
    if(vfsEvidence != NULL)
    {
        free(vfsEvidence);
    }

    return evidenceCollection;
}

bool AppraiseLinuxKernelMeasurement(const char* evidence)
{
    EvidenceBundle* bundle = (EvidenceBundle*)(evidence);
    bool result = true;

    while(!IsBundleNullBundle(bundle))
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

