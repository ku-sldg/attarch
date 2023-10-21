/*
** Functions for composing evidence in a consistent way to be transported via FFI
** Michael Neises
** 13 October 2023
*/

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "EvidenceBundle.h"

/*
** The whole point is to reduce all our disparate measurement results into a
** single collection of evidence. That collection is an array, and its entries
** are 128 bytes each (see EvidenceBundle.h for more).
*/

/**
 * Check if the provided EvidenceBundle is empty.
 *
 * @param bundle Pointer to the EvidenceBundle.
 * @return true if any field is empty, otherwise false.
 */
bool IsBundleEmpty(EvidenceBundle *bundle, bool logerror) {
    if (bundle == NULL)
    {
        return true;
    }

    bool isEmpty = true;

    void myprintf(char* str)
    {
        if(logerror)
        {
            printf(str);
        }
    }

    if (memcmp(bundle->type, "\0\0\0\0\0\0\0\0", 8) == 0) {
        myprintf("Type is empty.\n");
    } else {
        isEmpty = false;
    }

    if (memcmp(bundle->name, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 56) == 0) {
        myprintf("Name is empty.\n");
    } else {
        isEmpty = false;
    }

    if (memcmp(bundle->digest, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 64) == 0) {
        myprintf("Digest is empty.\n");
    } else {
        isEmpty = false;
    }

    return isEmpty;
}

/**
 * Create a new EvidenceBundle and validate its contents.
 *
 * @param type Pointer to an 8-byte type.
 * @param name Pointer to a 56-byte name.
 * @param digest Pointer to a 64-byte digest.
 * @return The created EvidenceBundle.
 */
EvidenceBundle CreateBundle(const char (*type)[8], const char (*name)[56], const char (*digest)[64]) {
    EvidenceBundle bundle;

    memcpy(bundle.type, *type, 8);
    memcpy(bundle.name, *name, 56);
    memcpy(bundle.digest, *digest, 64);

    if (IsBundleEmpty(&bundle, true)) {
        printf("CreateBundle Error: Some fields are empty.\n");
    }

    return bundle;
}

void PackBundleSingle(EvidenceBundle *collection_list, int collection_list_max_size, const EvidenceBundle *input_bundle) {
    // Find the first empty bundle in the collection_list
    int index = 0;
    while (index < collection_list_max_size) {
        if (IsBundleEmpty(&collection_list[index], false)) {
            collection_list[index] = *input_bundle; // append the bundle
            return;
        }
        index++;
    }

    // If the function has reached this point, the list is full
    printf("Error: Collection list is full, unable to append the new bundle.\n");
}


/**
 * Append bundles from the input list to the output list.
 *
 * @param output_list Pointer to the destination collection list.
 * @param output_list_max_size Maximum allowed size of the output list.
 * @param input_list Pointer to the source list.
 * @param input_list_size Number of bundles in the input list.
 */
void PackBundle(EvidenceBundle *collection_list, int collection_list_max_size, const EvidenceBundle *input_list, int input_list_size) {
    for (int i = 0; i < input_list_size; i++) {
        PackBundleSingle(collection_list, collection_list_max_size, &input_list[i]);
    }
}

/* void PackBundle(EvidenceBundle *output_list, int output_list_max_size, const EvidenceBundle *input_list, int input_list_size) { */
/*     if(input_list==NULL || output_list==NULL || output_list_max_size==0 || input_list_size==0) */
/*     { */
/*         return; */
/*     } */
/*     // Find the first empty bundle in the output_list */
/*     int index = 0; */
/*     while (index < output_list_max_size && !IsBundleEmpty(&output_list[index], false)) { */
/*         index++; */
/*     } */
/*     // Append bundles from input_list to output_list */
/*     int input_index = 0; */
/*     while (index < output_list_max_size && input_index < input_list_size) { */
/*         if (IsBundleEmpty(&output_list[index], false)) { */
/*             output_list[index] = input_list[input_index]; */
/*             input_index++; */
/*         } */
/*         index++; */
/*     } */
/* } */


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

    for (int i = 0; i < maxSize; i++) {
        if (!IsBundleEmpty(&collection[i], false)) {
            count++;
        }
        else
        {
            break;
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
void PrintCollection(EvidenceBundle *collection, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        printf("Entry %d:\n", i + 1);
        PrintBundle(&collection[i]);
        printf("\n");
    }
}

void ExportToByteString(EvidenceBundle* list, int list_size, char** evidence) {
    int bundle_size = sizeof(EvidenceBundle); // Size of each EvidenceBundle in bytes
    int total_size = bundle_size * list_size; // Total byte size for all EvidenceBundles

    // Allocate memory for all EvidenceBundles plus one byte for the null terminator
    *evidence = malloc(total_size + 1);
    if (*evidence == NULL) {
        // Handle memory allocation failure
        printf("Error: Memory allocation failed.\n");
        return;
    }

    // Copy each EvidenceBundle into the byte string
    for (int i = 0; i < list_size; i++) {
        memcpy(&(*evidence)[i * bundle_size], &list[i], bundle_size);
    }

    // Null terminate the byte string
    (*evidence)[total_size] = '\0';
}

