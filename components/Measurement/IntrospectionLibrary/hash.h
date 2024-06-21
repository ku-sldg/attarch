/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#ifndef MEASURE_HASH_H
#define MEASURE_HASH_H

#include <Hacl_Hash.h>
#include <stdbool.h> // for bool type
#include <stdint.h> // for uint64_t and other unsigned integer types
#include <stdio.h> // for printf (debugging)                    
#include <stdlib.h> // for malloc, calloc, free                   
#include "armv8a.h"

void PrintDigest(uint8_t (*digest)[DIGEST_NUM_BYTES]);

void ShaTest();

void HashMeasure(uint8_t* input, uint64_t offset, int inputLen, uint8_t (*output_digest)[DIGEST_NUM_BYTES]);

void HashExtend(uint8_t (*baseHash)[DIGEST_NUM_BYTES], uint8_t (*newHash)[DIGEST_NUM_BYTES]);

bool IsDigestEmpty(uint8_t (*digest)[DIGEST_NUM_BYTES]);

#endif

