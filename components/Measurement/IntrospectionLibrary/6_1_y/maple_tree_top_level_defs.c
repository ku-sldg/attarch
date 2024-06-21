#define MT_FLAGS_ALLOC_RANGE    0x01
#define MT_FLAGS_USE_RCU    0x02
#define MT_FLAGS_HEIGHT_OFFSET  0x02
#define MT_FLAGS_HEIGHT_MASK    0x7C
#define MT_FLAGS_LOCK_MASK  0x300
#define MT_FLAGS_LOCK_IRQ   0x100
#define MT_FLAGS_LOCK_BH    0x200
#define MT_FLAGS_LOCK_EXTERN    0x300

#define MAPLE_HEIGHT_MAX    31

#define MAPLE_RESERVED_RANGE    4096

#define MAPLE_ROOT_NODE 0x02
#define MAPLE_NODE_MASK 255UL
#define MAPLE_NODE_TYPE_MASK    0x0F
#define MAPLE_NODE_TYPE_SHIFT   0x03

/* 64bit sizes */
#define MAPLE_NODE_SLOTS    31  /* 256 bytes including ->parent */
#define MAPLE_RANGE64_SLOTS 16  /* 256 bytes */
#define MAPLE_ARANGE64_SLOTS    10  /* 240 bytes */
#define MAPLE_ARANGE64_META_MAX 15  /* Out of range for metadata */
#define MAPLE_ALLOC_SLOTS   (MAPLE_NODE_SLOTS - 1)

/* enode types */
#define MAS_START   ((struct maple_enode *)1UL)
#define MAS_ROOT    ((struct maple_enode *)5UL)
#define MAS_NONE    ((struct maple_enode *)9UL)
#define MAS_PAUSE   ((struct maple_enode *)17UL)
#define MA_ERROR(err) \
        ((struct maple_enode *)(((unsigned long)err << 2) | 2UL))

enum maple_type {
    maple_dense=0,
    maple_leaf_64=1,
    maple_range_64=2,
    maple_arange_64=3,
};

struct maple_metadata {
    unsigned char end;
    unsigned char gap;
};

struct maple_range_64 {
    struct maple_pnode *parent;
    unsigned long pivot[MAPLE_RANGE64_SLOTS - 1];
    union {
        void *slot[MAPLE_RANGE64_SLOTS];
        struct {
            void *pad[MAPLE_RANGE64_SLOTS - 1];
            struct maple_metadata meta;
        };
    };
};

/*
 * At tree creation time, the user can specify that they're willing to trade off
 * storing fewer entries in a tree in return for storing more information in
 * each node.
 *
 * The maple tree supports recording the largest range of NULL entries available
 * in this node, also called gaps.  This optimises the tree for allocating a
 * range.
 */
struct maple_arange_64 {
    struct maple_pnode *parent;
    unsigned long pivot[MAPLE_ARANGE64_SLOTS - 1];
    void *slot[MAPLE_ARANGE64_SLOTS];
    unsigned long gap[MAPLE_ARANGE64_SLOTS];
    struct maple_metadata meta;
};

struct maple_alloc {
    unsigned long total;
    unsigned char node_count;
    unsigned int request_count;
    struct maple_alloc *slot[MAPLE_ALLOC_SLOTS];
};


struct maple_node {
    union {
        struct {
            struct maple_pnode *parent;
            void *slot[MAPLE_NODE_SLOTS];
        };
        struct {
            void *pad;
            /* struct rcu_head rcu; */
            void *rcu_1;
            void *rcu_2;
            struct maple_enode *piv_parent;
            unsigned char parent_slot;
            enum maple_type type;
            unsigned char slot_len;
            unsigned int ma_flags;
        };
        struct maple_range_64 mr64;
        struct maple_arange_64 ma64;
        struct maple_alloc alloc;
    };
};

struct maple_tree {
    uint8_t ignore_me;
    void *ma_root;
    unsigned int ma_flags;
};

struct ma_state {
    struct maple_tree *tree;    /* The tree we're operating in */
    unsigned long index;        /* The index we're operating on - range start */
    unsigned long last;     /* The last index we're operating on - range end */
    struct maple_enode *node;   /* The node containing this entry */
    /* uint64_t node;   /1* The enode-style vaddr of the node containing this entry *1/ */
    unsigned long min;      /* The minimum index of this node - implied pivot min */
    unsigned long max;      /* The maximum index of this node - implied pivot max */
    struct maple_alloc *alloc;  /* Allocated nodes for this operation */
    /* uint64_t alloc;  /1* The virtual address of the Allocated nodes for this operation *1/ */
    unsigned char depth;        /* depth of tree descent during write */
    unsigned char offset;
    unsigned char mas_flags;
};

/* void PrintNodeData(char* memory_device, uint64_t node_paddr) */
/* { */
/*     printf("Begin Printing Node Data\n"); */
/*     introspectScanManyLongs(memory_device, node_paddr, 32, "complete node"); // the node is 256B */
/*     printf("Interpreting as arange_64\n"); */
/*     introspectScanManyLongs(memory_device, node_paddr+0, 1, "parent (0)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+8, 1, "pivot (8)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+80, 1, "slot (80)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+160, 1, "gap (160)"); */
/*     printf("maple_metadata: end = %u and gap = %u\n", ((char*)memory_device+240)[0], ((char*)memory_device+240)[1]); */
/*     printf("Interpreting as range_64\n"); */
/*     introspectScanManyLongs(memory_device, node_paddr+0, 1, "parent (0)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+8, 1, "pivot (8)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+128, 1, "slot (128)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+128, 1, "pad (128)"); */
/*     printf("maple_metadata: end = %u and gap = %u\n", ((char*)memory_device+248)[0], ((char*)memory_device+248)[1]); */
/*     printf("Interpreting as maple_alloc. Just kidding.\n"); */
/*     printf("Interpreting as maple_leaf\n"); */
/*     introspectScanManyLongs(memory_device, node_paddr+0, 1, "parent (0)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+8, 1, "slot (8)"); */
/*     printf("Interpreting as the other one\n"); */
/*     introspectScanManyLongs(memory_device, node_paddr+0, 1, "pad (0)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+8, 2, "rcu_head (8)"); */
/*     introspectScanManyLongs(memory_device, node_paddr+24, 1, "piv_parent (24)"); */
/*     printf("maple_type = %u\n", ((unsigned int *)((char*)memory_device+36))[0]); */
/*     printf("Done Printing Node Data\n"); */
/* } */

#define MA_STATE(name, mt, first, end)                  \
    struct ma_state name = {                    \
        .tree = mt,                     \
        .index = first,                     \
        .last = end,                        \
        .node = MAS_START,                  \
        .min = 0,                       \
        .max = ULONG_MAX,                   \
        .alloc = NULL,                      \
}

static inline unsigned int mt_height(const struct maple_tree *mt)
{
    return (mt->ma_flags & MT_FLAGS_HEIGHT_MASK) >> MT_FLAGS_HEIGHT_OFFSET;
}

static inline enum maple_type mte_node_type(const struct maple_enode *entry)
{
    enum maple_type mt = ((unsigned long)entry >> MAPLE_NODE_TYPE_SHIFT) &
        MAPLE_NODE_TYPE_MASK;
    /* printf("This node maple_type is %d\n", (int)mt); */
    return mt;
}

/* static inline bool ma_is_leaf(const enum maple_type type) */
static inline bool ma_is_leaf(const enum maple_type type)
{
    return type < maple_range_64;
    /* return type < 2; */
}

static inline bool mte_is_leaf(const struct maple_enode *entry)
{
    return ma_is_leaf(mte_node_type(entry));
}

/* /1* static inline struct maple_node *mte_to_node(const struct maple_enode *entry) *1/ */
/* // input: "enode-style" virtual address */
/* // return: physical offset(address) of the node in vm_memory */
/* static inline uint64_t mte_to_node(const struct maple_enode *entry) */
/* { */
/*     uint64_t node_raw_vaddr = (uint64_t)((unsigned long)entry & ~MAPLE_NODE_MASK); */
/*     printf("enode vaddr %p to %p\n", entry, node_raw_vaddr); */
/*     return node_raw_vaddr; */
/* } */

static inline struct maple_node *mte_to_node(const struct maple_enode *entry)
{
    return (struct maple_node *)((unsigned long)entry & ~MAPLE_NODE_MASK);
}


/*
 * We also reserve values with the bottom two bits set to '10' which are
 * below 4096
 */
static inline bool mt_is_reserved(const void *entry)
{
    return ((unsigned long)entry < MAPLE_RESERVED_RANGE) &&
        xa_is_internal(entry);
}

static const unsigned long mt_max[] = {
    [maple_dense]       = MAPLE_NODE_SLOTS,
    [maple_leaf_64]     = ULONG_MAX,
    [maple_range_64]    = ULONG_MAX,
    [maple_arange_64]   = ULONG_MAX,
};

/* static inline uint64_t mt_slot(const struct maple_tree *mt, uint64_t slots_list_paddr, unsigned char offset, char* memory_device) */
static inline void *mt_slot(const struct maple_tree *mt, void **slots, unsigned char offset)
{ 
    /* return rcu_dereference_check(slots[offset], mt_locked(mt)); */ // this is the original function definition
    return(slots[offset]);
}
