/* Tree validations */
static void mt_dump_node(const struct maple_tree *mt, void *entry,
        unsigned long min, unsigned long max, unsigned int depth, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest);

static void mt_dump_range(unsigned long min, unsigned long max,
              unsigned int depth)
{
    static const char spaces[] = "                                ";
    if (min == max)
    {
        /* printf("%.*s%lu: ", depth * 2, spaces, min); */
    }
    else
    {
        /* printf("%.*s%lu-%lu: ", depth * 2, spaces, min, max); */
    }
}

// return true if we made a measurement successfully
// return false otherwise
static bool MeasureThisVMA(void *entry, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    uint64_t this_entry_paddr = TranslateVaddr(memory_device, entry);
    /* printf("paddr = %llx\n", this_entry_paddr); */

    // Retrieve the first 2 longs of this_entry_paddr, which is a
    // vm_area_struct. long 1 is start. long 2 is end.
    // We could cast the pointer to a vm_area_struct pointer, but that would
    // cause a cascade of necessary struct to include.
    uint64_t start = ((uint64_t*)((char*)memory_device+this_entry_paddr))[0];
    uint64_t end = ((uint64_t*)((char*)memory_device+this_entry_paddr))[1];
    uint64_t size = end - start;

    uint64_t maybeELF = TranslationTableWalkSuppliedPGD(memory_device, start, pgd_paddr);
    if(0 == maybeELF)
    {
        /* printf("Address Translation Weirdly Failed\n"); */
        return;
    }

    if( size < RAM_SIZE
            && ((char*)memory_device+maybeELF)[0] == 0x7f
            && ((char*)memory_device+maybeELF)[1] == 'E'
            && ((char*)memory_device+maybeELF)[2] == 'L'
            && ((char*)memory_device+maybeELF)[3] == 'F' )
    {
        if(TryMeasureElfRodata(memory_device, maybeELF, pgd_paddr, rodataDigest))
        {
            // we got a good digest
            /* printf("found ELF at %p: ", maybeELF); */
            /* PrintDigest(rodataDigest); */
            return true;
        }
    }
    return false;
}

static void mt_dump_entry(void *entry, unsigned long min, unsigned long max, unsigned int depth, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    mt_dump_range(min, max, depth);
    if (xa_is_value(entry))
    {
        /* printf("value %ld (0x%lx) [%p]\n", xa_to_value(entry), xa_to_value(entry), entry); */
    }
    else if (xa_is_zero(entry))
    {
        /* printf("zero (%ld)\n", xa_to_internal(entry)); */
    }
    else if (mt_is_reserved(entry))
    {
        /* printf("UNKNOWN ENTRY (%p)\n", entry); */
    }
    else
    {
        /* printf("%p\n", entry); */
        if(entry>0)
        {
            bool success = MeasureThisVMA(entry, memory_device, pgd_paddr, rodataDigest);
        }
    }
}

static void mt_dump_range64(const struct maple_tree *mt, void *entry,
            unsigned long min, unsigned long max, unsigned int depth, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    struct maple_range_64 *node = (struct maple_range_64 *)(memory_device+TranslateVaddr(memory_device, mte_to_node(entry)));
    bool leaf = mte_is_leaf(entry);
    unsigned long first = min;
    int i;
    /* printf(" contents: "); */
    for (i = 0; i < MAPLE_RANGE64_SLOTS - 1; i++)
    {
        /* printf("%p %lu ", node->slot[i], node->pivot[i]); */
    }
    /* printf("%p\n", node->slot[i]); */
    for (i = 0; i < MAPLE_RANGE64_SLOTS; i++) {
        unsigned long last = max;
        if (i < (MAPLE_RANGE64_SLOTS - 1))
            last = node->pivot[i];
        else if (!node->slot[i] && max != mt_max[mte_node_type(entry)])
            break;
        if (last == 0 && i > 0)
            break;
        if (leaf)
            mt_dump_entry(mt_slot(mt, node->slot, i), first, last, depth + 1, memory_device, pgd_paddr, rodataDigest);
        else if (node->slot[i])
            mt_dump_node(mt, mt_slot(mt, node->slot, i), first, last, depth + 1, memory_device, pgd_paddr, rodataDigest);
        if (last == max)
            break;
        if (last > max) {
            /* printf("node %p last (%lu) > max (%lu) at pivot %d!\n", */
                    /* node, last, max, i); */
            break;
        }
        first = last + 1;
    }
}

static void mt_dump_arange64(const struct maple_tree *mt, void *entry,
            unsigned long min, unsigned long max, unsigned int depth, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    /* struct maple_arange_64 *node = &mte_to_node(entry)->ma64; */
    struct maple_arange_64 *node = (struct maple_arange_64 *)(memory_device+TranslateVaddr(memory_device, mte_to_node(entry)));
    bool leaf = mte_is_leaf(entry);
    unsigned long first = min;
    int i;
    /* printf(" contents: "); */
    for (i = 0; i < MAPLE_ARANGE64_SLOTS; i++)
    {
        /* printf("%lu ", node->gap[i]); */
    }
    /* printf("| %02X %02X| ", node->meta.end, node->meta.gap); */
    for (i = 0; i < MAPLE_ARANGE64_SLOTS - 1; i++)
    {
        /* printf("%p %lu ", node->slot[i], node->pivot[i]); */
    }
    /* printf("%p\n", node->slot[i]); */
    for (i = 0; i < MAPLE_ARANGE64_SLOTS; i++) {
        unsigned long last = max;
        if (i < (MAPLE_ARANGE64_SLOTS - 1))
            last = node->pivot[i];
        else if (!node->slot[i])
            break;
        if (last == 0 && i > 0)
            break;
        if (leaf)
            mt_dump_entry(mt_slot(mt, node->slot, i),
                    first, last, depth + 1, memory_device, pgd_paddr, rodataDigest);
        else if (node->slot[i])
            mt_dump_node(mt, mt_slot(mt, node->slot, i),
                    first, last, depth + 1, memory_device, pgd_paddr, rodataDigest); 
        if (last == max)
            break;
        if (last > max) { 
            /* printf("node %p last (%lu) > max (%lu) at pivot %d!\n", */
            /*         node, last, max, i); */
            break;
        }
        first = last + 1;
    }
}


static void mt_dump_node(const struct maple_tree *mt, void *entry,
        unsigned long min, unsigned long max, unsigned int depth, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    /* struct maple_node *node = mte_to_node(entry); */
    struct maple_node *node = (struct maple_node *)(memory_device+TranslateVaddr(memory_device, mte_to_node(entry)));

    unsigned int type = mte_node_type(entry);
    unsigned int i;
    mt_dump_range(min, max, depth);
    /* printf("node %p depth %d type %d", node, depth, type); */
    /* printf(" parent %p\n", node ? node->parent : NULL); */
    switch (type) {
    case maple_dense:
        /* printf("\n"); */
        for (i = 0; i < MAPLE_NODE_SLOTS; i++)
        {
            if (min + i > max)
            {
                /* printf("OUT OF RANGE: "); */
            }
            mt_dump_entry(mt_slot(mt, node->slot, i), min + i, min + i, depth, memory_device, pgd_paddr, rodataDigest);
        }
        break;
    case maple_leaf_64:
    case maple_range_64:
        mt_dump_range64(mt, entry, min, max, depth, memory_device, pgd_paddr, rodataDigest);
        break;
    case maple_arange_64:
        mt_dump_arange64(mt, entry, min, max, depth, memory_device, pgd_paddr, rodataDigest);
        break;
    default:
        printf(" UNKNOWN TYPE\n");
    }
}

void mt_dump(const struct maple_tree *mt, char* memory_device, uint64_t pgd_paddr, uint8_t* rodataDigest)
{
    /* void *entry = rcu_dereference_check(mt->ma_root, mt_locked(mt)); */
    void *entry = mt->ma_root;

    printf("maple_tree(%p) flags %X, height %u root %p\n",
         mt, mt->ma_flags, mt_height(mt), entry);
    if (!xa_is_node(entry))
        mt_dump_entry(entry, 0, 0, 0, memory_device, pgd_paddr, rodataDigest);
    else if (entry)
        mt_dump_node(mt, entry, 0, mt_max[mte_node_type(entry)], 0, memory_device, pgd_paddr, rodataDigest);
}

