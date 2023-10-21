/*
 * An introspection library for Linux 6.1.y
 * Michael Neises
 * 09 June 2023
 */

#define INTRO_KIMAGE_VADDR 0xffff800008000000

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#include "../hash.h"
#include "../KnownDigests.h"
#include "../configurations/include.h"
#include "AddressTranslation.c"
#include "common/common.c"
#include "measurementAndAppraisal.c"
#include "EvidenceBundle.h"
#include "EvidenceBundle.c"
#include "measurements.c"
#include "appraisals.c"

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
    EvidenceBundle rodataEvidence = InspectRodata((uint8_t*)memdev);
    EvidenceBundle* modulesEvidence = InspectModules((uint8_t*)memdev);
    EvidenceBundle* tasksEvidence = InspectTasks((uint8_t*)memdev);

    // Count the evidence entries so we know how much space we need.
    int numEvidenceEntries = 0;
    numRodataEvidences++; // we just have one
    numEvidenceEntries += numRodataEvidences;

    numModuleEvidences += GetCollectionLength(modulesEvidence, NUM_MODULE_DIGESTS * sizeof(EvidenceBundle));
    numEvidenceEntries += numModuleEvidences;

    numTaskEvidences += GetCollectionLength(tasksEvidence, NUM_TASKS * sizeof(EvidenceBundle));
    numEvidenceEntries += numTaskEvidences;
    
    // allocate memory
    EvidenceBundle* evidenceCollection = calloc(numEvidenceEntries, sizeof(EvidenceBundle));

    // pack the evidence into a single collection
    PackBundleSingle(evidenceCollection, numEvidenceEntries, &rodataEvidence);
    PackBundle(evidenceCollection, numEvidenceEntries, modulesEvidence, numModuleEvidences);
    PackBundle(evidenceCollection, numEvidenceEntries, tasksEvidence, numTaskEvidences);

    printf("\n\n==========\nFinal Evidence Collection\n==========\n");
    PrintCollection(evidenceCollection, numEvidenceEntries);

    // free the extra bundles
    if(modulesEvidence != NULL)
    {
        free(modulesEvidence);
    }
    if(tasksEvidence != NULL)
    {
        free(tasksEvidence);
    }
    
    return evidenceCollection;
}

bool AppraiseLinuxKernelMeasurement()
{

    /* AppraiseKernelRodata(); */
    /* AppraiseKernelModules(); */
    /* AppraiseLinuxTasks(); */

}

