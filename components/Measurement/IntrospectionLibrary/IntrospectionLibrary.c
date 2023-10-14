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
#include "measurements.c"
#include "appraisals.c"
#include "evidence_handling.c"

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

uint8_t* MeasureLinuxKernel()
{
    int numRodataEvidences = 0;
    int numModuleEvidences = 0;
    int numTaskEvidences = 0;

    // Execute the measurements
    uint8_t* rodataEvidence = InspectRodata((uint8_t*)memdev);

    uint8_t* modulesEvidence = InspectModules((uint8_t*)memdev);

    uint8_t* tasksEvidence = InspectTasks((uint8_t*)memdev);
    /* PrintCollection(rodataEvidence, 1); */

    // Count the evidence entries so we know how much space we need.
    int numEvidenceEntries = 0;
    if(rodataEvidence != NULL)
    {
        //count it
        numRodataEvidences++;
        PrintCollection(rodataEvidence, numRodataEvidences);
        numEvidenceEntries += numRodataEvidences;
        printf("rodata get. entries now %d\n", numEvidenceEntries);
    }
    if(modulesEvidence != NULL)
    {
        //count it
        numModuleEvidences += GetLengthOfCollectionSansEmpties(modulesEvidence, NUM_MODULE_DIGESTS * EVIDENCE_ENTRY_SIZE);
        PrintCollection(modulesEvidence, numModuleEvidences);
        numEvidenceEntries += numModuleEvidences;
        printf("modules get. entries now %d\n", numEvidenceEntries);
    }
    if(tasksEvidence != NULL)
    {
        //count it
        numTaskEvidences += GetLengthOfCollectionSansEmpties(tasksEvidence, NUM_TASKS * EVIDENCE_ENTRY_SIZE);
        PrintCollection(tasksEvidence, numTaskEvidences);
        numEvidenceEntries += numTaskEvidences;
        printf("Tasks get. entries now %d\n", numEvidenceEntries);
    }
    
    printf("DEBUG: YOU HAD %d ENTRIES\n", numEvidenceEntries);

    // allocate memory
    uint8_t* evidenceCollection = calloc(numEvidenceEntries, EVIDENCE_ENTRY_SIZE);
    int max_size = numEvidenceEntries * EVIDENCE_ENTRY_SIZE;

    // pack the evidence into a single collection
    if(rodataEvidence != NULL)
    {
        PackIntoCollection(evidenceCollection, rodataEvidence, max_size);
    }
    if(modulesEvidence != NULL)
    {
        PackIntoCollection(evidenceCollection + EVIDENCE_ENTRY_SIZE * numRodataEvidences, modulesEvidence, max_size);
    }
    if(tasksEvidence != NULL)
    {
        PackIntoCollection(evidenceCollection + EVIDENCE_ENTRY_SIZE * (numRodataEvidences + numModuleEvidences), tasksEvidence, max_size);
    }
    
    return evidenceCollection;
}

bool AppraiseLinuxKernelMeasurement()
{

    /* AppraiseKernelRodata(); */
    /* AppraiseKernelModules(); */
    /* AppraiseLinuxTasks(); */

}

