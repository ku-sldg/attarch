#define SWAPPER_OFFSET 0
#define PAGE_OFFSET 0xFFFF800000000000
#define KIMAGE_VADDR 0xFFFF000008000000
#define BIT(x) (1UL << (x))
bool is_linear_map_address(uint64_t vaddr)
{
    return( vaddr & BIT(INTRO_VA_BITS - 1) );
}
