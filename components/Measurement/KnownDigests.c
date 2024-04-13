/* Previously registered sha512 digests
 * Michael Neises
 * 11 July 2022
 */

#include "KnownDigests.h"

void HexToByteString(const char (*input_digest)[128], uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    int value;
    for(int i=0; i < DIGEST_NUM_BYTES && sscanf(((char*)input_digest) + i * 2, "%2x", &value); i++)
    {
        ((uint8_t*)output_digest)[i] = value;
    }
}

#include "generated_digests.c"

bool IsThisAKnownDigest(uint8_t (*input_digest)[DIGEST_NUM_BYTES])
{
    uint8_t (*known)[INTRO_NUM_DIGESTS * DIGEST_NUM_BYTES] = calloc(INTRO_NUM_DIGESTS, DIGEST_NUM_BYTES);
    int numKnownDigests = register_digests(known);
    bool isKnown = false;
    for(int i=0; i < numKnownDigests; i++)
    {
        isKnown = true;
        for(int j=0; j<DIGEST_NUM_BYTES; j++)
        {
            if(((uint8_t*)known)[i*DIGEST_NUM_BYTES + j] != ((uint8_t*)input_digest)[j])
            {
                isKnown = false;
                break;
            }
        }
        if(isKnown)
        {
            free(known);
            return true;
        }
    }
    free(known);
    return false;
}

void RenderDigestDeclaration(char (*name)[INTRO_MODULE_NAME_LEN], uint8_t (*digest)[DIGEST_NUM_BYTES])
{
    printf("\tconst char %s[] = \"", (*name));
    PrintDigest(digest);        
    printf("\";\n");
    printf("\tHexToByteString(&%s, &(((uint8_t*)digests)[DIGEST_NUM_BYTES*(numDigests++)]));\n", (*name));
}



