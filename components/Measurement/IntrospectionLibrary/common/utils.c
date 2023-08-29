/*
 * An introspection library for Linux 4.9.307
 * a few little helpful things for investigative development
 * Michael Neises
 * 2 May 2023
 */

void introLog(int args, ...)
{
    if(true)
    {
        printf("DEBUG: Introspection: ");
        va_list ptr;
        va_start(ptr, args);
        for(int i=0; i<args; i++)
        {
            printf("%s", va_arg(ptr,char*));
        }
    }
}

void introspectScan(uint8_t* memory_device, int* head, int size, char* name)
{
    printf("%s: ", name);
    //for(int i=0; i<size; i++)
    for(int i=size-1; i>=0; i--)
    {
        printf("%02X",((char*)memory_device+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanChar(uint8_t* memory_device, int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    //for(int i=size; i>0; i--)
    {
        printf("%c",((char*)memory_device+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanInt(uint8_t* memory_device, int* head, char* name)
{
    printf("%s: ", name);
    int* val = (int*)((char*)memory_device+*head);
    printf("%d : %u", *val, *val);
    printf("\n");
    *head += 4;
}
void introspectScanLong(uint8_t* memory_device, int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memory_device+*head);
    printf("%lu", *val);
    printf("\n");
    *head += 8;
}

void introspectScanManyLongs(uint8_t* memory_device, uint64_t offset, int numLongs, char* label)
{
    /* printf("\nas llx"); */
    printf("Label: %s", label);
    for(int i=0; i<numLongs; i++)
    {
        if( i%(8) == 0 )
        {
            printf("\nOffset: %04d | ", i*8);
        }
        /* printf("%016llx ", ((uint64_t*)(memory_device+offset))[i]); */
        printf("%p ", ((uint64_t*)(memory_device+offset))[i]);
    }
    printf("\n\n");
}

void introspectScanAddr(uint8_t* memory_device, int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memory_device+*head);
    printf("%p : %016X", *val, *val);
    printf("\n");
    *head += 8;
}

void introspectScanMaybeChar(uint8_t* memory_device, uint64_t offset, int num_bytes, char* label)
{
    printf("Label: %s: ", label);
    for(int i=0; i<num_bytes; i++)
    {
        uint8_t thisChar = ((char*)memory_device+offset)[i];
        if( 0 < thisChar && thisChar < 128 )
        {
            printf("%c",((char*)memory_device+offset)[i]);
        }
        else
        {
            /* printf("%02X",((char*)memory_device+offset)[i]); */
        }
    }
    printf("\n");
}

