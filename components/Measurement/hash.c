/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */
/* Depends on: IntrospectionLibrary/utils.c */

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
    //PrintDigest(output);
    printf("\n");
    free(output);
}

void HashMeasure(uint8_t* input, int inputLen, uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    Hacl_Hash_SHA2_hash_512(input, inputLen, ((uint8_t*)output_digest));
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

void MeasureUserPage(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = TranslationTableWalk(memory_device, pageVaddr);
    /* for(int i=0; i<4096; i++) */
    /* { */
    /*     printf("%c", ((char*)memory_device+pagePaddr)[i]); */
    /* } */
    /* printf("\n\n"); */
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}

