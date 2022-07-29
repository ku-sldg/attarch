/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#include "hash.h"
#include <stdlib.h>

void PrintDigest(uint8_t* digest)
{
    for(int i=0; i<64; i++)
    {
        //if(i>0&&i%16==0){printf("\n");}
        printf("%02X", digest[i]);
    }
    printf("\n");
}

void ShaTest()
{
    uint8_t* output = malloc(64);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    printf("SHA512 of \"abc\" is:\n");
    PrintDigest(output);
}

void HashMeasure(uint8_t* input, int inputLen, uint8_t* output)
{
    Hacl_Hash_SHA2_hash_512(input, inputLen, output);
}

void HashHashes(uint8_t* hashList, int numHashes, uint8_t* output)
{
    if(numHashes < 2)
    {
        for(int i=0; i<64; i++)
        {
            output[i] = hashList[i];
        }
        return;
    }
    int cmphash (const void* a, const void* b)
    {
        uint8_t aByte = 0;
        uint8_t bByte = 0;
        for(int i=0; i<64; i++)
        {
            printf("a\n");
            aByte = ((uint8_t*)a)[i];
            bByte = ((uint8_t*)b)[i];
            printf("b\n");
            if(aByte < bByte)
            {
                return -1;
            }
            if(aByte > bByte)
            {
                return 1;
            }
        }
        return 0;
    }
    qsort(hashList, numHashes, 64, cmphash);
    HashMeasure(hashList, 64*numHashes, output);
}

