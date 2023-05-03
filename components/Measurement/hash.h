/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#ifndef MEASURE_HASH_H
#define MEASURE_HASH_H

#include <string.h>
#include <stdlib.h>
#include <Hacl_Hash.h>
#include "configurations/include.h"

void PrintDigest(uint8_t (*digest)[DIGEST_NUM_BYTES]);

void ShaTest();

void HashMeasure(uint8_t* input, int inputLen, uint8_t (*output_digest)[DIGEST_NUM_BYTES]);

void HashHashes(uint8_t* hashList, int numHashes, uint8_t (*output_digest)[DIGEST_NUM_BYTES]);

bool IsDigestEmpty(uint8_t (*digest)[DIGEST_NUM_BYTES]);

void MeasureKernelPage(uint8_t* memdev, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr);

void MeasureUserPage(uint8_t* memdev, uint8_t (*output_digest)[DIGEST_NUM_BYTES], uint64_t pageVaddr);

#endif

