/* a few functions for appraisal of kernel module hash digests
** Michael Neises
** 11 July 2022
*/

#include "KnownDigests.h"

void AppraiseKernelModule(uint8_t* input_digest, bool* result, char* name)
{
    uint8_t** digests = calloc(numKnownDigests, sizeof(uint8_t*));
    for(int i=0; i< numKnownDigests; i++)
    {
        digests[i] = calloc(64, sizeof(uint8_t));
    }

    GetKnownModuleDigests(digests);

    bool isMatched = false;
    for(int i=0; i < numKnownDigests; i++)
    {
        isMatched = true;
        for(int j=0; j<64; j++)
        {
            if(digests[i][j] != input_digest[j])
            {
                isMatched = false;
                break;
            }
        }
        if(isMatched)
        {
            *result = true;
            return;
        }
    }
    *result = false;
}

