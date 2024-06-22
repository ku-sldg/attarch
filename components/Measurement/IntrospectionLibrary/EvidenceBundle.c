/*
** Functions for composing evidence in a consistent way to be transported via FFI
** Michael Neises
** 13 October 2023
*/

#include "EvidenceBundle.h"

/*
** The whole point is to reduce all our disparate measurement results into a
** single collection of evidence. That collection is an array, and its entries
** are 128 bytes each (see EvidenceBundle.h for more).
*/

bool IsFinalBundle(EvidenceBundle bundle)
{
    for(int i=0; i<8; i++)
    {
        if(((uint8_t*)(&bundle))[i] != '\1')
        {
            return false;
        }
    }
    return true;
}

/**
 * Create a new EvidenceBundle and validate its contents.
 *
 * @param type Pointer to an 8-byte type.
 * @param name Pointer to a 56-byte name.
 * @param digest Pointer to a 64-byte digest.
 * @return The created EvidenceBundle.
 */
EvidenceBundle* AllocBundle(const char (*type)[8], const char (*name)[56], const uint8_t (*digest)[64]) {
    EvidenceBundle* bundle = calloc(1,sizeof(EvidenceBundle));
    for(int i=0; i<8; i++)
    {
        bundle->type[i] = (*type)[i];
    }
    for(int i=0; i<56; i++)
    {
        bundle->name[i] = (*name)[i];
    }
    for(int i=0; i<64; i++)
    {
        bundle->digest[i] = (*digest)[i];
    }
    return bundle;
}

/**
 * Return the number of EvidenceBundle entries in a collection.
 *
 * @param collection Pointer to the collection list.
 * @param maxSize Maximum allowed size of the collection.
 * @return Number of non-empty EvidenceBundles in the collection.
 */
int GetCollectionLength(EvidenceBundle *collection, int maxSize) {
    if(collection==NULL)
    {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < maxSize; i++)
    {
        if(IsFinalBundle(collection[i]))
        {
            break;
        }
        else
        {
            count++;
        }
    }
    return count;
}

/**
 * Print the contents of a single EvidenceBundle.
 *
 * @param bundle Pointer to the EvidenceBundle.
 */
void PrintBundle(EvidenceBundle *bundle) {
    if(bundle==NULL)
    {
        printf("ERROR: Tried to print NULL as an EvidenceBundle\n");
        return;
    }
    printf("Type: %.*s\n", 8, bundle->type);
    printf("Name: %.*s\n", 56, bundle->name);
    printf("Digest: ");
    for (int i = 0; i < 64; i++) {
        printf("%02X", (unsigned char) bundle->digest[i]);
    }
    printf("\n");
}

/**
 * Print the contents of a collection of EvidenceBundles.
 *
 * @param collection Pointer to the collection list.
 * @param numEntries Number of entries in the collection.
 */
void PrintCollection(EvidenceBundle *collection) {
    int numEntries = GetCollectionLength(collection, 32);
    for (int i = 0; i < numEntries; i++) {
        printf("Entry %d:\n", i + 1);
        PrintBundle(&collection[i]);
        printf("\n");
    }
}

void ExportToByteString(EvidenceBundle* list, int list_size, char* evidence) {
    int bundle_size = sizeof(EvidenceBundle); // Size of each EvidenceBundle in bytes
    int total_size = bundle_size * list_size; // Total byte size for all EvidenceBundles
    if (evidence == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }
    // Copy each EvidenceBundle into the byte string
    for (int i = 0; i < list_size; i++) {
        for(int j=0; j< bundle_size; j++)
        {
            evidence[i * bundle_size + j] = ((uint8_t*)(&(list[i])))[j];
        }
    }
}

void AppendBundle(EvidenceBundle** list, EvidenceBundle bundle)
{
    int len = GetCollectionLength(*list, 32);
    EvidenceBundle* ret = calloc(len+2, sizeof(EvidenceBundle));
    for(int i=0; i<len; i++)
    {
        ret[i] = (*list)[i];
    }
    free(*list);
    ret[len] = bundle;
    ret[len+1] = nullEvidenceBundle;
    *list = ret;
}
