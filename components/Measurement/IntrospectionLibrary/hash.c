/* Michael Neises */
/* Some functions regarding hashing and digests */
/* April 2024 */

#include "hash.h"

void PrintDigest(uint8_t (*digest)[DIGEST_NUM_BYTES])
{
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        //if(i>0&&i%16==0){printf("\n");}
        printf("%02X", ((uint8_t*)digest)[i]);
    }
}

void ShaTest()
{
    uint8_t* output = malloc(DIGEST_NUM_BYTES);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    printf("SHA512 of \"abc\" is:\n");
    PrintDigest(output);
    printf("\n");
    free(output);
}

void HashMeasure(uint8_t* base_pointer, uint64_t offset, int inputLen, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    if(RAM_SIZE < offset)
    {
        printf("Bad Address. Check #define LINUX_VERSION in introspect_component.c, which must match the version of Linux being used.\n");
        return;
    }
    Hacl_Hash_SHA2_hash_512(base_pointer+offset, inputLen, ((uint8_t*)output_digest));
}

void HashExtend(uint8_t (*baseHash)[DIGEST_NUM_BYTES], uint8_t (*newHash)[DIGEST_NUM_BYTES])
{
    uint8_t* longHash = calloc(1, 2*DIGEST_NUM_BYTES);
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        longHash[i] = ((uint8_t*)baseHash)[i];
    }
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        longHash[i+DIGEST_NUM_BYTES] = ((uint8_t*)newHash)[i];
    }
    HashMeasure(longHash, 0, 2*DIGEST_NUM_BYTES, baseHash);
    free(longHash);
}

bool IsDigestEmpty(uint8_t (*digest)[DIGEST_NUM_BYTES])
{
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        if(((uint8_t*)digest)[i] != '\0')
        {
            return false;
        }
    }
    return true;
}


