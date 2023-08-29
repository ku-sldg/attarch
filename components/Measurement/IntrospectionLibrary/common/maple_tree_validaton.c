/* File: maple_tree_validation.c */
/* Some validation functions over maple trees. A work-in-progress. */

uint64_t DereferenceMyOffset(char* memory_device, uint64_t struct_paddr, int offset)
{
    return ((uint64_t*)((char*)memory_device+struct_paddr+offset))[0];
}

/*
 * mas_mn() - Get the maple state node.
 * @mas: The maple state
 *
 * Return: the maple node (not encoded - bare pointer).
 */
/* static inline struct maple_node *mas_mn(const struct ma_state *mas) */
// return: virtual address of the mas->node in vm_memory
static inline uint64_t mas_mn(const struct ma_state *mas, char* memory_device)
{
    printf("mas->node is %p\n", mas->node);
    return mte_to_node(mas->node, memory_device);
}



/*
 * ma_pivots() - Get a pointer to the maple node pivots.
 * @node - the maple node
 * @type - the node type
 *
 * In the event of a dead node, this array may be %NULL
 *
 * Return: A pointer to the maple node pivots
 */
/* static inline unsigned long *ma_pivots(struct maple_node *node, */
/*                        enum maple_type type) */
// this returns the offset(physical address) of the list of pivot array in vm_memory
static inline uint64_t ma_pivots(uint64_t node_vaddr, int maple_type, char* memory_device)
{
    uint64_t node_paddr = TranslateVaddr(memory_device, node_vaddr);
    printf("Scanning what I believe to be a maple node. Pivots start either at 0 or at 8. There can be as many as 10 pivots.\n");
    printf("vaddr: %p\npaddr: %p\n", node_vaddr, node_paddr);
    /* introspectScanManyLongs(memory_device, node_paddr); */
    uint64_t pivot_list_paddr = 0;
    switch (maple_type) {
    case maple_arange_64:
        /* return node->ma64.pivot; */
        /* pivot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+8))[0]; */
        pivot_list_paddr = node_paddr + 8;
        printf("arange pivot paddr is %p\n", pivot_list_paddr);
        /* return TranslateVaddr(memory_device, pivot_list_paddr); */
        return pivot_list_paddr;
    case maple_range_64:
    case maple_leaf_64:
        /* return node->mr64.pivot; */
        /* pivot_list_paddr = ((uint64_t*)((char*)memory_device+node_paddr+8))[0]; */
        pivot_list_paddr = node_paddr + 8;
        printf("leaf/range pivot paddr is %p\n", pivot_list_paddr);
        /* return TranslateVaddr(memory_device, pivot_list_paddr); */
        return pivot_list_paddr;
    case maple_dense:
        return 0;
    }
    return 0;
}


/*
 * ma_slots() - Get a pointer to the maple node slots.
 * @mn: The maple node
 * @mt: The maple node type
 *
 * Return: A pointer to the maple node slots
 */
/* static inline void __rcu **ma_slots(struct maple_node *mn, enum maple_type mt) */
// this returns the offset(physical address) of the list of slot-pointers in vm_memory
static inline uint64_t ma_slots(uint64_t node_vaddr, int maple_type, char* memory_device)
{
    uint64_t slot_vaddr = 0;
    uint64_t node_paddr = TranslateVaddr(memory_device, node_vaddr); // this completes reasonably

    printf("begin\n");
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+80))[0];
    printf("arange slot vaddr is %p\n", slot_vaddr);
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+128))[0];
    printf("leaf/range slot vaddr is %p\n", slot_vaddr);
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+8))[0];
    printf("dense slot vaddr is %p\n", slot_vaddr);
    printf("dingo\n");
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+80))[0] & ~MAPLE_NODE_MASK;
    printf("arange slot vaddr is %p\n", slot_vaddr);
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+128))[0] & ~MAPLE_NODE_MASK;
    printf("leaf/range slot vaddr is %p\n", slot_vaddr);
    slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+8))[0] & ~MAPLE_NODE_MASK;
    printf("dense slot vaddr is %p\n", slot_vaddr);
    printf("end\n");


    printf("top of ma_slots\n");
    /* introspectScanManyLongs(memory_device, node_paddr); */

    
    switch (maple_type) {
    default:
    case maple_arange_64:
        /* return mn->ma64.slot; */
        // follow the node_offset. ma64 is at offset 0 in the node. slot is at
        // offset 80 in ma64.
        slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+80))[1];
        printf("arange slot vaddr is %p\n", slot_vaddr);
        /* slot_vaddr = slot_vaddr >> 4; // try getting rid of the lowest valued hex */
        /* slot_vaddr = slot_vaddr << 4; // we were getting an address ending in "c" and the contents seemed to be oddly offset, so... */
        // TODO we're getting better results this way, but why would the
        // introspected address be "c" too high?
        // This could be the node type, and we're masking it off ?
        /* printf("arange slot vaddr masked is %p\n", slot_vaddr); */
        return TranslateVaddr(memory_device, slot_vaddr);
    case maple_range_64: // can we really let this drop, as in the source code?
    case maple_leaf_64:
        /* return mn->mr64.slot; */
        // follow the node_offset. mr64 is at offset 0 in the node. slot is at
        // offset 128 in mr64.
        slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+128))[0];
        printf("leaf/range slot vaddr is %p\n", slot_vaddr);
        return TranslateVaddr(memory_device, slot_vaddr);
    case maple_dense:
        // follow the node_offset. The offset of slot in maple_node is 8.
        /* return mn->slot; */
        slot_vaddr = ((uint64_t*)((char*)memory_device+node_paddr+8))[0];
        printf("dense slot vaddr is %p\n", slot_vaddr);
        /* printf("dense slot vaddr masked is %p\n", slot_vaddr); */
        return TranslateVaddr(memory_device, slot_vaddr);
    }
}

/* static inline void *mt_slot(const struct maple_tree *mt, */
/*         void __rcu **slots, unsigned char offset) */
static inline uint64_t mt_slot(const struct maple_tree *mt,
        uint64_t slots_list_paddr, unsigned char offset, char* memory_device)
{ 
    /* return rcu_dereference_check(slots[offset], mt_locked(mt)); */
    /* return slots[offset]; */

    /* printf("Scanning what I think is the slots list...\n"); */
    introspectScanManyLongs(memory_device, slots_list_paddr, 4, "slots plain ass");
    /* printf("Scanning what I think is the slots list junior...\n"); */
    introspectScanManyLongs(memory_device, slots_list_paddr & ~MAPLE_NODE_MASK, 4, "slots after masking");

    uint64_t thisSlotVaddr = ((uint64_t*)((char*)memory_device+(slots_list_paddr & ~MAPLE_NODE_MASK)))[offset];
    /* uint64_t thisSlotVaddr = ((uint64_t*)((char*)memory_device+slots_list_paddr))[offset]; */
    // TODO thisSlotVaddr is a serious misnomer. This function is bad.
    // The value held by thisSlotVaddr is actually just some long long

    // print out slots information?
    /* thisSlotVaddr = thisSlotVaddr >> 4; */
    /* thisSlotVaddr = thisSlotVaddr << 4; */
    printf("mt_slot returning %p\n", thisSlotVaddr);
    return thisSlotVaddr;
}

/*
 * mas_slot() - Get the slot value when not holding the maple tree lock.
 * @mas: The maple state
 * @slots: The pointer to the slots
 * @offset: The offset into the slots array to fetch
 *
 * Return: The entry stored in @slots at the @offset
 */
/* static inline void *mas_slot(struct ma_state *mas, void __rcu **slots, */
                 /* unsigned char offset) */
static inline uint64_t mas_slot(struct ma_state *mas, uint64_t slots_list_paddr,
                 unsigned char offset, char* memory_device)
{
    return mt_slot(mas->tree, slots_list_paddr, offset, memory_device);
}


/*
 * ma_dead_node() - check if the @enode is dead.
 * @node: The raw maple node vaddr
 *
 * Return: true if dead, false otherwise.
 */
/* static inline bool ma_dead_node(const struct maple_node *node) */
static inline bool ma_dead_node(uint64_t node_vaddr, char* memory_device)
{
    uint64_t node_paddr = TranslateVaddr(memory_device, node_vaddr);
    /* struct maple_node *parent; */
    uint64_t parentEnodeVaddr = 0;
    uint64_t parentNodeVaddr = 0;

    /* Do not reorder reads from the node prior to the parent check */
    /* smp_rmb(); */ // do not translate. memory barrier.

    /* parent = (void *)((unsigned long) node->parent & ~MAPLE_NODE_MASK); */
    parentEnodeVaddr = ((uint64_t*)((char*)memory_device+node_paddr))[0];
    parentNodeVaddr = ((unsigned long)parentEnodeVaddr & ~MAPLE_NODE_MASK);

    return parentNodeVaddr == node_vaddr;
}

/*
 * mte_dead_node() - check if the @enode is dead.
 * @enode: The enode-style vaddr
 *
 * Return: true if dead, false otherwise.
 */

static inline bool mte_dead_node(char* memory_device, uint64_t enode)
{
    printf("begin mte_dead_node\n");
    uint64_t parentEnodeVaddr = 0;
    uint64_t parentNodeVaddr = 0;
    uint64_t nodeVaddr = 0;
    uint64_t nodePaddr = 0;

    nodeVaddr = ((unsigned long)enode & ~MAPLE_NODE_MASK);
    nodePaddr = TranslateVaddr(memory_device, nodeVaddr);
    /* /1* Do not reorder reads from the node prior to the parent check *1/ */
    /* smp_rmb(); */ // this is a memory barrier or "memory fence." Will not translate.

    parentEnodeVaddr = ((uint64_t*)((char*)memory_device+nodePaddr))[0];
    parentNodeVaddr = ((unsigned long)parentEnodeVaddr & ~MAPLE_NODE_MASK);

    /* printf("nodeVaddr %llx\n", nodeVaddr); */
    /* printf("nodePaddr %llx\n", nodePaddr); */
    /* printf("parentEnodeVaddr %llx\n", parentEnodeVaddr); */
    /* printf("parentNodeVaddr %llx\n", parentNodeVaddr); */

    return (parentNodeVaddr == nodeVaddr);
}


/*
 * mas_first_entry() - Go the first leaf and find the first entry.
 * @mas: the maple state.
 * @limit: the maximum index to check.
 * @*r_start: Pointer to set to the range start.
 *
 * Sets mas->offset to the offset of the entry, r_start to the range minimum.
 *
 * Return: The first entry or MAS_NONE.
 */
/* static inline void *mas_first_entry(struct ma_state *mas, struct maple_node *mn, */
/*         unsigned long limit, enum maple_type mt) */
static inline uint64_t mas_first_entry(struct ma_state *mas, uint64_t maple_node_vaddr,
        unsigned long limit, enum maple_type mt, char* memory_device)

{
    printf("\n\nmas_first_entry starto\n\n");

    PrintNodeData(memory_device, TranslateVaddr(memory_device, maple_node_vaddr));
    /* unsigned long max; */
    /* unsigned long *pivots; */
    /* void __rcu **slots; */
    /* void *entry = NULL; */
    uint64_t max = 0;
    uint64_t pivots_paddr = 0; // are these vaddrs or paddrs ???
    uint64_t slots_list_paddr = 0;
    uint64_t entry = 0;

    printf("passed declarations\n");

    mas->index = mas->min;
    if (mas->index > limit)
    {
        printf("index greater than limit\n");
        goto none;
    }

    printf("passed conditional\n");
    max = mas->max;
    mas->offset = 0;
    printf("entering loop\n");
    while (!ma_is_leaf(mt)) {
    /*     MT_BUG_ON(mas->tree, mte_dead_node(mas->node)); */ //Debug code. Will not translate.
        printf("entering ma_slots\n");
        slots_list_paddr = ma_slots(maple_node_vaddr, mt, memory_device); // TODO at least it appears slots_list_paddr points to consistent data boot-over-boot
        // TODO however, the data is not what I was expecting. I was expecting
        // a list of pointers, but I don't think I've gotten that...
        printf("entering mas_slot\n");
        entry = mas_slot(mas, slots_list_paddr, 0, memory_device); // TODO I think this call is bad bc entry is malformed. I think "entry" corresponds to "enode".
        printf("entry is %p\n", entry);
        // TODO mas_slot should return an enode vaddr, but I don't know if slots[offset]
        // really is that, given that slots has type void**. The contents of
        // slots_list do not appear to be addresses at all.


        printf("entering ma_pivots\n");
        pivots_paddr = ma_pivots(maple_node_vaddr, mt, memory_device);
        printf("entering ma_dead_node\n");
        if (ma_dead_node(maple_node_vaddr, memory_device))
            return NULL;
        printf("entering DereferenceMyOffset\n");
        max = DereferenceMyOffset(memory_device, pivots_paddr, 0); /*     max = pivots[0]; */
        printf("max is %llx\n", max);
        mas->node = entry;
        printf("entering mas_mn\n");
        maple_node_vaddr = mas_mn(mas, memory_device);
        printf("maple_node_vaddr is %p we'll scan that location now.\n", maple_node_vaddr);
        PrintNodeData(memory_device, TranslateVaddr(memory_device, maple_node_vaddr));
        /* introspectScanManyLongs(memory_device, TranslateVaddr(memory_device, maple_node_vaddr)); */
        printf("entering mte_node_type\n");
        mt = mte_node_type(mas->node);
        printf("node type is %d\n", (int)mt);
        printf("finishing round of loop\n");
    }
    /* MT_BUG_ON(mas->tree, mte_dead_node(mas->node)); */ // Debug code. Will not translate.

    mas->max = max;
    slots_list_paddr = ma_slots(maple_node_vaddr, mt, memory_device);
    entry = mas_slot(mas, slots_list_paddr, 0, memory_device);
    if (ma_dead_node(maple_node_vaddr, memory_device))
    {
        printf("dead node, returning NULL\n");
        return NULL;
    }

    /* Slot 0 or 1 must be set */
    if (mas->index > limit)
    {
        printf("index out of limit\n");
        goto none;
    }

    if (entry)
    {
        printf("returning entry\n");
        return entry;
    }

    mas->offset = 1;
    entry = mas_slot(mas, slots_list_paddr, 1, memory_device);
    pivots_paddr = ma_pivots(maple_node_vaddr, mt, memory_device);
    if (ma_dead_node(maple_node_vaddr, memory_device))
    {
        printf("dead node, returning NULL\n");
        return NULL;
    }

    /* mas->index = pivots[0] + 1; */ //TODO see next line
    mas->index = DereferenceMyOffset(memory_device, pivots_paddr, 0) + 1; //TODO I'm not certain about the nature of this incrementation.
    if (mas->index > limit)
    {
        printf("index out of limit\n");
        goto none;
    }

    if (entry)
    {
        printf("returning entry\n");
        return entry;
    }

none:
    printf("went to none\n");
    if (!ma_dead_node(maple_node_vaddr, memory_device))
    {
        printf("wasn't dead\n");
        mas->node = MAS_NONE;
    }
    printf("returning NULL\n");
    return NULL;
}

static inline struct maple_enode *mas_start(char* memory_device, struct ma_state *mas)
{
    if (mas->node == MAS_START) {
        struct maple_enode *root;

        mas->min = 0;
        mas->max = ULONG_MAX;
        mas->depth = 0;

retry:
        root = mas->tree->ma_root;
        /* Tree with nodes */
        bool xa_is_internal = ((unsigned long)root & 3) == 2;
        bool is_real_pointer = (unsigned long)root > 4096;
        if (xa_is_internal && is_real_pointer) {
            mas->depth = 1;
            mas->node = (uint64_t)((unsigned long)root & ~MAPLE_ROOT_NODE);
            mas->offset = 0;
            if (mte_dead_node(memory_device, mas->node))
                goto retry;
            printf("not dead\n");
            return NULL;
        }

        /* empty tree */
        if (!root) {
            mas->node = MAS_NONE;
            mas->offset = MAPLE_NODE_SLOTS;
            printf("empty tree\n");
            return NULL;
        }

        /* Single entry tree */
        mas->node = MAS_ROOT;
        mas->offset = MAPLE_NODE_SLOTS;

        /* Single entry tree. */
        if (mas->index > 0)
            printf("single entry tree\n");
            return NULL;

        return root;
    }

    return NULL;
}

bool ValidateMapleTree(char* memory_device, uint64_t maple_tree_offset)
{
    printf("\nValidating Maple Tree\n");

    struct maple_tree mt = ((struct maple_tree *)((char*)memory_device + maple_tree_offset))[0];

    unsigned char end;
    MA_STATE(mas, &mt, 0, 0);
    /* rcu_read_lock(); */ // I will not translate this into an introspective procedure.

    mas_start(memory_device, &mas);
    printf("\nsome node is %p\n", mas.node);
    if( (mas.node == MAS_NONE || mas.node == MAS_ROOT) )
        goto done;

    mas_first_entry(&mas, mas_mn(&mas, memory_device), ULONG_MAX, mte_node_type(mas.node), memory_device);
    while(mas.node != MAS_NONE)
    {
    /*     MT_BUG_ON(mas.tree, mte_dead_node(mas.node)); */
    /*     if (!mte_is_root(mas.node)) { */
    /*         end = mas_data_end(&mas); */
    /*         if ((end < mt_min_slot_count(mas.node)) && */
    /*             (mas.max != ULONG_MAX)) { */
    /*             pr_err("Invalid size %u of %p\n", end, */
    /*             mas_mn(&mas, memory_device)); */
    /*             MT_BUG_ON(mas.tree, 1); */
    /*         } */

    /*     } */
    /*     mas_validate_parent_slot(&mas); */
    /*     mas_validate_child_slot(&mas); */
    /*     mas_validate_limits(&mas); */
    /*     if (mt_is_alloc(mt)) */
    /*         mas_validate_gaps(&mas); */
    /*     mas_dfs_postorder(&mas, ULONG_MAX); */
        break;
    }
    /* mt_validate_nulls(mt); */
done:
    /* rcu_read_unlock(); */ // I will not translate this into an introspective procedure.

    printf("ending validation of maple tree\n");
}
