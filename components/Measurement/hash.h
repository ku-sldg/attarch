/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#ifndef MEASURE_HASH_H
#define MEASURE_HASH_H

#include <string.h>
#include <stdlib.h>
/* #include <Hacl_Hash.h> */

void PrintDigest(uint8_t* digest);

void RenderDigestDeclaration(char* name, uint8_t* digest);

void ShaTest();

void HashMeasure(uint8_t* input, int inputLen, uint8_t* output);

void HashHashes(uint8_t* hashList, int numHashes, uint8_t* output);

bool IsDigestEmpty(uint8_t* input);

void MeasureKernelPage(uint8_t* memdev, uint8_t* digest, uint64_t pageVaddr);

void MeasureUserPage(uint8_t* memdev, uint8_t* digest, uint64_t pageVaddr);

#endif

