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

void HashHashes(uint8_t (*hashListPointers[NUM_RODATA_PAGES])[DIGEST_NUM_BYTES], uint8_t (*output_digest)[DIGEST_NUM_BYTES])
{
    uint8_t* hashList = calloc(NUM_RODATA_PAGES, DIGEST_NUM_BYTES);
    for(int i=0; i<NUM_RODATA_PAGES; i++)
    {
        for(int j=0; j<DIGEST_NUM_BYTES; j++)
        {
            hashList[i*DIGEST_NUM_BYTES + j] = hashListPointers[i][j];
        }
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
    qsort(hashList, NUM_RODATA_PAGES, DIGEST_NUM_BYTES, cmphash);
    HashMeasure(hashList, DIGEST_NUM_BYTES*NUM_RODATA_PAGES, output_digest);
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

void MeasureKernelPage(uint8_t* memdev, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = intro_virt_to_phys(pageVaddr-0x8000000);
    HashMeasure( ((char*)memdev+pagePaddr), PAGE_SIZE, output_digest );
}

void MeasureUserPage(uint8_t* memdev, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = TranslateVaddr(pageVaddr);
    /* for(int i=0; i<4096; i++) */
    /* { */
    /*     printf("%c", ((char*)memdev+pagePaddr)[i]); */
    /* } */
    /* printf("\n\n"); */
    HashMeasure( ((char*)memdev+pagePaddr), PAGE_SIZE, output_digest );
}

