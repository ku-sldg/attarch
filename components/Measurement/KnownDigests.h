/* Previously registered sha512 digests
 * includes: task rodata, kernel module rodata
 * Michael Neises
 * 11 July 2022
 */

#ifndef INTRO_KNOWN_DIGESTS_H
#define INTRO_KNOWN_DIGESTS_H

#include "configurations/include.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// render our convenient digest hex strings into usable form
void HexToByteString(const char (*input_digest)[129], uint8_t (*output_digest)[DIGEST_NUM_BYTES]);

// copy our known digests into the argument passed in.
// return the number of known digests
int GetKnownDigests(uint8_t (*digests)[INTRO_NUM_DIGESTS * DIGEST_NUM_BYTES]);

// check a digest against our known digests
bool IsThisAKnownDigest(uint8_t (*input_digest)[DIGEST_NUM_BYTES]);

// print out c code for if you want to declare a new golden digest
void RenderDigestDeclaration(char* name, uint8_t (*digest)[DIGEST_NUM_BYTES]);

#endif
