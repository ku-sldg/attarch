/*
 * An introspection library for Linux 4.9.307
 * Exhaustive Interpretation of a task struct
 * Michael Neises
 * 21 July 2022
 */

/* Elf specs from */ 
/* https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html */
/* https://web.mit.edu/freebsd/head/sys/sys/elf64.h */

struct elf64header {
    uint64_t      RamOffset;
    unsigned char   e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
};

struct elf64phdr {
    uint64_t elf_offset; /* paddr for the elf file */
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;       /* Segment file offset */
    uint64_t p_vaddr;       /* Segment virtual address */
    uint64_t p_paddr;       /* Segment physical address */
    uint64_t p_filesz;     /* Segment size in file */
    uint64_t p_memsz;      /* Segment size in memory */
    uint64_t p_align;      /* Segment alignment, file & memory */
};

struct elf64shdr {
    uint64_t elf_offset; /* paddr for the elf file */
    uint32_t sh_name;       /* Section name, index in string tbl */
    uint32_t sh_type;       /* Type of section */
    uint64_t sh_flags;     /* Miscellaneous section attributes */
    uint64_t sh_addr;       /* Section virtual addr at execution */
    uint64_t sh_offset;      /* Section file offset */
    uint64_t sh_size;      /* Size of section in bytes */
    uint32_t sh_link;       /* Index of another section */
    uint32_t sh_info;       /* Additional section information */
    uint64_t sh_addralign; /* Section alignment */
    uint64_t sh_entsize;   /* Entry size if section holds table */
};

int RetThenInc(int* valToIncrement, int incValue)
{
    int valToReturn = *valToIncrement;
    *valToIncrement += incValue;
    return valToReturn;
}

struct elf64phdr CollectProgramHeader(uint64_t phAddr, uint64_t elfAddr)
{
    struct elf64phdr head;
    head.elf_offset = elfAddr;
    int hdrptr = 0;
    head.p_type = ((uint32_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,4)))[0];
    head.p_flags = ((uint32_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,4)))[0];
    head.p_offset = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    head.p_vaddr = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    head.p_paddr = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    head.p_filesz = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    head.p_memsz = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    head.p_align = ((uint64_t*)((char*)memdev+phAddr+RetThenInc(&hdrptr,8)))[0];
    return head;
}

void PrintProgramHeaderData(struct elf64phdr* header)
{
    printf("p_type : %08X\n", header->p_type);
    printf("p_flags : %08X\n", header->p_flags);
    printf("p_offset : %016X\n", header->p_offset);
    printf("p_vaddr : %016X\n", header->p_vaddr);
    printf("p_paddr : %016X\n", header->p_paddr);
    printf("p_filesz : %016X\n", header->p_filesz);
    printf("p_memsz : %016X\n", header->p_memsz);
    printf("p_align : %016X\n", header->p_align);
}

struct elf64shdr CollectSectionHeader(uint64_t shAddr, uint64_t elfAddr)
{
    /* printf("shaddr:\n\t%016X\n\t%p\n", shAddr, shAddr); */
    struct elf64shdr head;
    head.elf_offset = elfAddr;
    int hdrptr = 0;
    head.sh_name = ((uint32_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,4)))[0];
    head.sh_type = ((uint32_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,4)))[0];
    head.sh_flags = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    head.sh_addr = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    head.sh_offset = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    head.sh_size = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    head.sh_link = ((uint32_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,4)))[0];
    head.sh_info = ((uint32_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,4)))[0];
    head.sh_addralign = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    head.sh_entsize = ((uint64_t*)((char*)memdev+shAddr+RetThenInc(&hdrptr,8)))[0];
    return head;
}

struct elf64header CollectElfHeaderData(uint64_t elfAddr)
{
    int elfPtr = 0;
    struct elf64header head;
    head.RamOffset = elfAddr;
    for(int i=0; i<16; i++)
    {
        head.e_ident[i] = ((char*)memdev+elfAddr)[RetThenInc(&elfPtr,1)];
    }
    head.e_type = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_machine = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_version = ((uint32_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,4)))[0];
    head.e_entry = ((uint64_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,8)))[0];
    head.e_phoff = ((uint64_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,8)))[0];
    head.e_shoff = ((uint64_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,8)))[0];
    head.e_flags = ((uint32_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,4)))[0];
    head.e_ehsize = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_phentsize = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_phnum = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_shentsize = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_shnum = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    head.e_shstrndx = ((uint16_t*)((char*)memdev+elfAddr+RetThenInc(&elfPtr,2)))[0];
    return head;
}

bool IsThisTheHeaderName(struct elf64header* elf, struct elf64shdr* shdr, uint64_t shstrtabPtr, char* nameGuess)
{
    if(shstrtabPtr+(shdr->sh_name) < 0x8001000)
    {
        char* shName = ((char*)memdev+shstrtabPtr+(shdr->sh_name));
        char shNameSubstring[strlen(nameGuess)];
        memcpy(&shNameSubstring, shName, strlen(nameGuess));
        return(strcmp(shNameSubstring, nameGuess)==0);
    }
    return false;
}

uint64_t GetHeaderName(struct elf64shdr* shdr, uint64_t shstrtabPtr)
{
    return shstrtabPtr+(shdr->sh_name);
}

char* GetOneHeaderName(struct elf64header* elf, struct elf64shdr* shdr)
{
    uint64_t shstrtabHdrPtr = elf->RamOffset
                        + elf->e_shoff
                        + (elf->e_shentsize * elf->e_shstrndx);
    struct elf64shdr shstrtabhdr = CollectSectionHeader(shstrtabHdrPtr, elf->RamOffset);
    uint64_t shstrtabPtr = elf->RamOffset + shstrtabhdr.sh_offset;
    if(shstrtabPtr+(shdr->sh_name) < 0x8001000)
    {
        char* shName = ((char*)memdev+shstrtabPtr+(shdr->sh_name));
        return shName;
        printf("Got Name: %s\n", shName);
    }
    printf("sh_offset was huge: %lu\n", shstrtabhdr.sh_offset);
    return "ERROR: NAME NOT FOUND";

}

void PrintSectionHeaderData(struct elf64shdr* header, bool printAll)
{
    if(header->sh_addr == 0 && header->sh_offset == 0)
    {
        return;
    }
    struct elf64header elfHeader = CollectElfHeaderData(header->elf_offset);
    if(printAll)
    {
        printf("sh_type : %08X\n", header->sh_type);
        printf("sh_flags : %016X\n", header->sh_flags);
        printf("sh_addr : %016X\n", header->sh_addr);
        printf("sh_offset : %016X\n", header->sh_offset);
        printf("sh_size : %016X\n", header->sh_size);
        printf("sh_link : %08X\n", header->sh_link);
        printf("sh_info : %08X\n", header->sh_info);
        printf("sh_addralign : %016X\n", header->sh_addralign);
        printf("sh_entsize : %016X\n", header->sh_entsize);
        printf("\n");
    }
    printf("sh_name : %s\n", GetOneHeaderName(&elfHeader, header));
}

void PrintElfHeaderData(struct elf64header* header)
{
    printf("name: %016X\n", header->e_ident);
    printf("RAM Offset: %016X\n", header->RamOffset);
    printf("type: %d\n", header->e_type);
    printf("machine: %d\n", header->e_machine);
    printf("version: %d\n", header->e_version);
    printf("entry: %p\n", header->e_entry);
    printf("phoff: %p\n", header->e_phoff);
    printf("shoff: %p\n", header->e_shoff);
    printf("flags: %d\n", header->e_flags);
    printf("ehsize: %d\n", header->e_ehsize);
    printf("phentsize: %d\n", header->e_phentsize);
    printf("phnum: %d\n", header->e_phnum);
    printf("shentsize: %d\n", header->e_shentsize);
    printf("shnum: %d\n", header->e_shnum);
    printf("shstrndx: %d\n", header->e_shstrndx);
}

void CrawlProgramHeaders(struct elf64header* elf, uint8_t* outputDigest)
{
    uint64_t segmentPtr = elf->RamOffset + elf->e_phoff;
    uint8_t* segmentDigests = calloc(elf->e_phnum, 64);
    int numDigests = 0;
    for(int i=0; i<elf->e_phnum; i++)
    {
        struct elf64phdr thisPhdr = CollectProgramHeader(segmentPtr, elf->RamOffset);
        if(!(thisPhdr.p_flags & 2)) // "if this progrm header refers to a non-writable segment..."
        {
            HashMeasure( ((char*)memdev+thisPhdr.p_vaddr), thisPhdr.p_memsz, &segmentDigests[numDigests*64] );
            numDigests++;
        }
        segmentPtr+=elf->e_phentsize;
    }
    HashMeasure(segmentDigests, 64*numDigests, outputDigest);
    free(segmentDigests);
}

bool TryMeasureElfRodata(uint64_t elfAddr, uint64_t pgd, uint8_t* outputDigest)
{
    struct elf64header elf = CollectElfHeaderData(elfAddr);
    /* PrintElfHeaderData(&elf); */
    CrawlProgramHeaders(&elf, outputDigest);
    return true;
}
