#define SWAPPER_OFFSET 0x80000
// this 48 := CONFIG_ARM64_VA_BITS
#define PAGE_OFFSET (-(1ULL << 48))
// this 47 := vabits_actual - 1
#define PAGE_END (-(1ULL << 47))
#define KIMAGE_VADDR 0xFFFF800010000000
bool is_linear_map_address(uint64_t vaddr)
{
    return ((vaddr ^ PAGE_OFFSET) < (PAGE_END - PAGE_OFFSET));
}
