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

bool IsFinalBundle(EvidenceBundle bundle);

EvidenceBundle* AllocBundle(const char (*type)[8], const char (*name)[56], const uint8_t (*digest)[64]);

int GetCollectionLength(EvidenceBundle *collection, int maxSize);

void PrintBundle(EvidenceBundle *bundle);

void PrintCollection(EvidenceBundle *collection);

void ExportToByteString(EvidenceBundle* list, int list_size, char* evidence);

void AppendBundle(EvidenceBundle** list, EvidenceBundle bundle);

#endif // EVIDENCE_BUNDLE_H

