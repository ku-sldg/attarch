/* Michael Neises */
/* Some functions regarding hashing and digests */
/* 28 July 2022 */

#ifndef MEASURE_HASH_H
#define MEASURE_HASH_H

#include <string.h>
#include <Hacl_Hash.h>

void PrintDigest(uint8_t* digest);

void ShaTest();

void HashMeasure(uint8_t* input, int inputLen, uint8_t* output);

void HashHashes(uint8_t* hashList, int numHashes, uint8_t* output);

#endif
