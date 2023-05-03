/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */
/* Depends on: IntrospectionLibrary/utils.c */

#include "hash.h"

void PrintDigest(uint8_t* digest)
{
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        //if(i>0&&i%16==0){printf("\n");}
        printf("%02X", digest[i]);
    }
}

void RenderDigestDeclaration(char* name, uint8_t* digest)
{
    printf("\tconst char %s[] = \"", name);
    PrintDigest(digest);        
    printf("\";\n");
    printf("\tHexToByteString(&%s, &digests[DIGEST_NUM_BYTES*(numDigests++)]);\n", name);
}

void ShaTest()
{
    uint8_t* output = malloc(64);
    Hacl_Hash_SHA2_hash_512("abc", 3, output);
    printf("SHA512 of \"abc\" is:\n");
    PrintDigest(output);
    printf("\n");
    free(output);
}

void HashMeasure(uint8_t* input, int inputLen, uint8_t* output)
{
    Hacl_Hash_SHA2_hash_512(input, inputLen, output);
}

void HashHashes(uint8_t* hashList, int numHashes, uint8_t* output)
{
    if(numHashes < 2)
    {
        for(int i=0; i<DIGEST_NUM_BYTES; i++)
        {
            output[i] = hashList[i];
        }
        return;
    }
    int cmphash (const void* a, const void* b)
    {
        uint8_t aByte = 0;
        uint8_t bByte = 0;
        for(int i=0; i<DIGEST_NUM_BYTES; i++)
        {
            aByte = ((uint8_t*)a)[i];
            bByte = ((uint8_t*)b)[i];
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
    // qsort always seems to Instruction Fault for some raisin
    qsort(hashList, numHashes, DIGEST_NUM_BYTES, cmphash);
    HashMeasure(hashList, DIGEST_NUM_BYTES*numHashes, output);
}

bool IsDigestEmpty(uint8_t* input)
{
    for(int i=0; i<DIGEST_NUM_BYTES; i++)
    {
        if(input[i] != '\0')
        {
            return false;
        }
    }
    return true;
}

void MeasureKernelPage(uint8_t* memdev, uint8_t* digest, uint64_t pageVaddr)
{
    uint64_t pagePaddr = intro_virt_to_phys(pageVaddr-0x8000000);
    HashMeasure( ((char*)memdev+pagePaddr), PAGE_SIZE, digest );
}

void MeasureUserPage(uint8_t* memdev, uint8_t* digest, uint64_t pageVaddr)
{
    uint64_t pagePaddr = TranslateVaddr(pageVaddr);
    /* for(int i=0; i<4096; i++) */
    /* { */
    /*     printf("%c", ((char*)memdev+pagePaddr)[i]); */
    /* } */
    /* printf("\n\n"); */
    HashMeasure( ((char*)memdev+pagePaddr), PAGE_SIZE, digest );
}

