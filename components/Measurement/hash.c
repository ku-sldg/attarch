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

/* These measurement functions are "hash" things in a sense, */
/*       but more appropriately they are "measurement" things. */
/*       The reason they are here is because it was once awkward to include files the right way. */
/*       The current solution is not idea, but it should allow these functions to be moved into the introspection library. */
// for Linux 4.x.y
void MeasureKernelPageLinux4(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    // what's with this offset?
    // this hard-coded thing only works for 4.9.y apparently, but not 4.14 or 4.19
    // which is curious
    //uint64_t pagePaddr = intro_virt_to_phys(pageVaddr-0x8000000);
    uint64_t pagePaddr = pageVaddr-0xFFFF000008000000; // this is simply vaddr - kimage_vaddr
    //uint64_t pagePaddr = intro_virt_to_phys(pageVaddr);
    /* printf("translating kernel rodata page vaddr: %llx\n", pageVaddr); */
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}
// for Linux 5.x.y and 6.x.y
void MeasureKernelPageLinux5(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    /* uint64_t pagePaddr = TranslateVaddr(memory_device, pageVaddr); */
    uint64_t pagePaddr = pageVaddr-INTRO_KIMAGE_VADDR; // this is simply vaddr - kimage_vaddr
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}

void MeasureUserPage(uint8_t* memory_device, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr)
{
    uint64_t pagePaddr = TranslateVaddr(memory_device, pageVaddr);
    /* for(int i=0; i<4096; i++) */
    /* { */
    /*     printf("%c", ((char*)memory_device+pagePaddr)[i]); */
    /* } */
    /* printf("\n\n"); */
    HashMeasure( ((char*)memory_device+pagePaddr), INTRO_PAGE_SIZE, output_digest );
}

