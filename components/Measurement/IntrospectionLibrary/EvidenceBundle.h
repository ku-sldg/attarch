#ifndef EVIDENCE_BUNDLE_H
#define EVIDENCE_BUNDLE_H

typedef struct {
    char type[8];          // 8 bytes for type
    char name[56];         // 56 bytes for name
    char digest[64];       // 64 bytes for digest
} EvidenceBundle;

extern char RODATA_TYPE[8];
extern char MODULE_TYPE[8];
extern char TASK_TYPE[8];

#endif // EVIDENCE_BUNDLE_H

