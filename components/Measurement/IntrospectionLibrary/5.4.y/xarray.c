#define XA_CHUNK_SHIFT 6
#define XA_CHUNK_SIZE 1 << XA_CHUNK_SHIFT
#define BITS_PER_LONG 64
#define XA_SLOTS 40

/**
 * xa_to_value() - Get value stored in an XArray entry.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: The value stored in the XArray entry.
 */
unsigned long xa_to_value(uint64_t entry)
{
    return entry >> 1;
}

/**
 * xa_is_value() - Determine if an entry is a value.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: True if the entry is a value, false if it is a pointer.
 */
bool xa_is_value(uint64_t entry)
{
    return entry & 1;
}

/*
 * xa_mk_internal() - Create an internal entry.
 * @v: Value to turn into an internal entry.
 *
 * Internal entries are used for a number of purposes.  Entries 0-255 are
 * used for sibling entries (only 0-62 are used by the current code).  256
 * is used for the retry entry.  257 is used for the reserved / zero entry.
 * Negative internal entries are used to represent errnos.  Node pointers
 * are also tagged as internal entries in some situations.
 *
 * Context: Any context.
 * Return: An XArray internal entry corresponding to this value.
 */
uint64_t xa_mk_internal(uint64_t v)
{
    return ((v << 2) | 2);
}

/*
 * xa_is_internal() - Is the entry an internal entry?
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: %true if the entry is an internal entry.
 */
bool xa_is_internal(uint64_t entry)
{
    return (entry & 3) == 2;
}

#define XA_ZERO_ENTRY		xa_mk_internal(257)

/**
 * xa_is_zero() - Is the entry a zero entry?
 * @entry: Entry retrieved from the XArray
 *
 * The normal API will return NULL as the contents of a slot containing
 * a zero entry.  You can only see zero entries by using the advanced API.
 *
 * Return: %true if the entry is a zero entry.
 */
bool xa_is_zero(uint64_t entry)
{
    return entry == XA_ZERO_ENTRY;
}

typedef uint64_t xa_node;

xa_node xa_to_node(uint64_t entry)
{
    return (xa_node)(entry - 2);
}

bool xa_is_node(uint64_t entry)
{
    return xa_is_internal(entry) && entry > 4096;
}

uint64_t xa_mk_sibling(unsigned int offset)
{
    return xa_mk_internal(offset);
}

/**
 * xa_is_sibling() - Is the entry a sibling entry?
 * @entry: Entry retrieved from the XArray
 *
 * Return: %true if the entry is a sibling entry.
 */
bool xa_is_sibling(uint64_t entry)
{
    return xa_is_internal(entry) &&
        (entry < xa_mk_sibling(XA_CHUNK_SIZE - 1));
}

#define XA_RETRY_ENTRY		xa_mk_internal(256)

/**
 * xa_is_retry() - Is the entry a retry entry?
 * @entry: Entry retrieved from the XArray
 *
 * Return: %true if the entry is a retry entry.
 */
bool xa_is_retry(uint64_t entry)
{
    return entry == XA_RETRY_ENTRY;
}

/*
 * xa_to_internal() - Extract the value from an internal entry.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: The value which was stored in the internal entry.
 */
uint64_t xa_to_internal(uint64_t entry)
{
    return entry >> 2;
}

uint64_t xa_to_sibling(uint64_t entry)
{
    return xa_to_internal(entry);
}

void xa_dump_node(uint8_t* memdev, xa_node node, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    /* printf("Printing at node address: %llx\n", node); */
    unsigned char offset = 0;
    offset = (memdev+node)[2];
    uint64_t litmus = ((uint64_t*)(memdev+node+48))[0];
    uint64_t addr_base = 0xFFFF000000000000;
    if(addr_base <= litmus && litmus < addr_base+RAM_SIZE)
    {
        // only measure "data containing" nodes
        // that is, exclude nodes that only refer to other nodes
        return;
    }

    /* for(int i=8*6; i<8*72; i++) // should be 8*5? why does the first slot not match? */
    /* { */
    /*     if(i%64==0) */
    /*     { */
    /*         printf("\n\t"); */
    /*     } */
    /*     else if(i%8==0) */
    /*     { */
    /*         printf(" "); */
    /*     } */
    /*     printf("%02X", (memdev+node)[i]); */ 
    /* } */
    /* printf("\n"); */

    uint8_t (*tempDigest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    HashMeasure(memdev+node+8*6, 63, tempDigest);
    HashExtend(digest, tempDigest);
    free(tempDigest);

}

void xa_dump_index(uint64_t index, unsigned int shift)
{
    /* if (!shift) */
    /*     printf("%lu: ", index); */
    /* else if (shift >= BITS_PER_LONG) */
    /*     printf("0-%lu: ", ~0UL); */
    /* else */
    /*     printf("%lu-%lu: ", index, index | ((1UL << shift) - 1)); */
}

uint8_t GetShift(uint8_t* memdev, uint64_t node)
{
    uint8_t shift = (memdev+node)[0];
    return shift;
}

uint64_t GetSlots(uint8_t* memdev, uint64_t node, int i)
{
    uint64_t result = ((uint64_t*)(memdev+node+XA_SLOTS))[i];
    result = TranslationTableWalk(memdev, result);
    return result;
}

void xa_dump_entry(uint8_t* memdev, uint64_t entry, uint64_t index, uint64_t shift, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    if (entry == 0)
        return;

    xa_dump_index(index, shift);

    if (xa_is_node(entry)) {
        if (shift == 0) {
            /* printf("%px\n", entry); //never happens? */
        } else {
            uint64_t i;
            xa_node node = xa_to_node(entry);
            xa_dump_node(memdev, node, digest);
            for (i = 0; i < XA_CHUNK_SIZE; i++)
            {
                xa_dump_entry(memdev, GetSlots(memdev, node, i),
                        index + (i << GetShift(memdev, node)), GetShift(memdev, node), digest);
            }
        }
    }
    else if (xa_is_value(entry))
    {
        /* printf("value %ld (0x%lx) [%px]\n", xa_to_value(entry), */
        /*         xa_to_value(entry), entry); */
    }
    else if (!xa_is_internal(entry))
    {
        /* printf("%px\n", entry); */
    }
    else if (xa_is_retry(entry))
    {
        /* printf("retry (%ld)\n", xa_to_internal(entry)); */
    }
    else if (xa_is_sibling(entry))
    {
        /* printf("sibling (slot %ld)\n", xa_to_sibling(entry)); */
    }
    else if (xa_is_zero(entry))
    {
        /* printf("zero (%ld)\n", xa_to_internal(entry)); */
    }
    else
    {
        /* printf("UNKNOWN ENTRY (%px)\n", entry); */
    }
}

void xa_dump(uint8_t* memdev, uint64_t xa_head_ptr, uint8_t(*digest)[DIGEST_NUM_BYTES])
{
    uint64_t shift = 0;
    if (xa_is_node(xa_head_ptr))
    {
        shift = GetShift(memdev, xa_to_node(xa_head_ptr)) + XA_CHUNK_SHIFT;
    }
    xa_dump_entry(memdev, xa_head_ptr, 0, shift, digest);
}

