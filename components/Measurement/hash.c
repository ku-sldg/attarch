/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#include "hash.h"

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
