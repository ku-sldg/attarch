#ifndef EVIDENCE_BUNDLE_H
#define EVIDENCE_BUNDLE_H

#include <stdbool.h> // for bool type
#include <stdint.h> // for uint64_t and other unsigned integer types
#include <stdio.h> // for printf (debugging)                    
#include <stdlib.h> // for malloc, calloc, free                   

typedef struct {
    char type[8];          // 8 bytes for type
    char name[56];         // 56 bytes for name
    uint8_t digest[64];       // 64 bytes for digest
} EvidenceBundle;

extern const char RODATA_TYPE[8];
extern const char MODULE_TYPE[8];
extern const char TASK_TYPE[8];

static EvidenceBundle nullEvidenceBundle = {
    .type = {1, 1, 1, 1, 1, 1, 1, 1},  // Set all bytes in 'type' to 1
    .name = {0},                       // The rest of the bytes in 'name' will be automatically set to 0
    .digest = {0}                      // The bytes in 'digest' will be automatically set to 0
};

bool memoryCompare(uint8_t* first, uint8_t* second, int numBytes);

bool IsBundleNullBundle(EvidenceBundle* bundle);

bool IsBundleEmpty(EvidenceBundle *bundle, bool logerror);

EvidenceBundle CreateBundle(const char (*type)[8], const char (*name)[56], const uint8_t (*digest)[64]);

void PackBundleSingle(EvidenceBundle *collection_list, int collection_list_max_size, const EvidenceBundle *input_bundle);

void PackBundle(EvidenceBundle *collection_list, int collection_list_max_size, const EvidenceBundle *input_list, int input_list_size);

int GetCollectionLength(EvidenceBundle *collection, int maxSize);

void PrintBundle(EvidenceBundle *bundle);

void PrintCollection(EvidenceBundle *collection, int numEntries);

void ExportToByteString(EvidenceBundle* list, int list_size, char* evidence);

#endif // EVIDENCE_BUNDLE_H

