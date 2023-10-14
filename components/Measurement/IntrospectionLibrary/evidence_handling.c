/*
** Functions for composing evidence in a consistent way to be transported via FFI
** Michael Neises
** 13 October 2023
*/

/*
** The whole point is to reduce all our disparate measurement results into a
** single collection of evidence. That collection is an array, and its entries
** are EVIDENCE_ENTRY_SIZE bytes each (that's 128 now. see measurements.c for
** more). The byte breakdown is as follows:
** 8 : string type | 56 : string name | 64 : hash digest
*/

bool IsEntryEmpty(uint8_t* entry)
{
    for(int i=0; i<EVIDENCE_ENTRY_SIZE; i++)
    {
        if( 0 < entry[i] )
        {
            return false;
        }
    }
    return true;
}

void PackIntoCollection(uint8_t* collection, uint8_t* entry, int collection_max_size)
{

    uint8_t* head = collection;
    int offset = 0;

    while( !IsEntryEmpty(head) )
    {
        offset += EVIDENCE_ENTRY_SIZE;
        if(offset+EVIDENCE_ENTRY_SIZE < collection_max_size)
        {
            head = collection + offset;
        }
        else
        {
            printf("WARNING: Tried to add an evidence entry to a full collection! The entry was lost!\n");
            return;
        }
    }

    for(int i=0; i<EVIDENCE_ENTRY_SIZE; i++)
    {
        collection[offset+i] = entry[i];
    }

}

int GetLengthOfCollectionSansEmpties(uint8_t* collection, int collection_max_size)
{
    uint8_t* head = collection;
    int length = 0;
    while( !IsEntryEmpty(head) )
    {
        length++;
        if((length*EVIDENCE_ENTRY_SIZE) < (collection_max_size-EVIDENCE_ENTRY_SIZE))
        {
            head = collection + (length*EVIDENCE_ENTRY_SIZE);
        }
        else
        {
            break;
        }
    }
    return length;
}

void PrintEntry(uint8_t* entry)
{
    printf("Evidence:\n");
    printf("\tEvidence Type: %s\n", entry);
    printf("\tParticular Name: %s\n", entry+8);
    printf("\tSHA512 Digest: ");
    for(int i=64; i<EVIDENCE_ENTRY_SIZE; i++)
    {
        printf("%02X", entry[i]);
    }
    printf("\n");
}

void PrintCollection(uint8_t* collection, int numEntries)
{
    for(int i=0; i<numEntries; i++)
    {
        PrintEntry(collection+ (EVIDENCE_ENTRY_SIZE * i));
    }
}
