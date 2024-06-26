/* Previously registered sha512 digests
 * includes: task rodata, kernel module rodata
 * Michael Neises
 * 11 July 2022
 */

#ifndef INTRO_KNOWN_DIGESTS_H
#define INTRO_KNOWN_DIGESTS_H

#include "armv8a.h"

#include <stdlib.h> // for malloc, calloc, free
#include <stdbool.h> // for bool type
#include <stdint.h> // for unsigned integer types
#include <stdio.h> // for printf (debugging)

// render our convenient digest hex strings into usable form
// why is this 129?
void HexToByteString(const char (*input_digest)[128], uint8_t (*output_digest)[DIGEST_NUM_BYTES]);

// copy our known digests into the argument passed in.
// return the number of known digests
int GetKnownDigests(uint8_t (*digests)[INTRO_NUM_DIGESTS * DIGEST_NUM_BYTES]);

// check a digest against our known digests
bool IsThisAKnownDigest(uint8_t (*input_digest)[DIGEST_NUM_BYTES]);

// print out c code for if you want to declare a new golden digest
void RenderDigestDeclaration(char (*name)[INTRO_MODULE_NAME_LEN], uint8_t (*digest)[DIGEST_NUM_BYTES]);

#endif
