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

void introspectScan(int* head, int size, char* name)
{
    printf("%s: ", name);
    //for(int i=0; i<size; i++)
    for(int i=size-1; i>=0; i--)
    {
        printf("%02X",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanChar(int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    //for(int i=size; i>0; i--)
    {
        printf("%c",((char*)memdev+*head)[i]);
    }
    printf("\n");
    *head += size;
}
void introspectScanInt(int* head, char* name)
{
    printf("%s: ", name);
    int* val = (int*)((char*)memdev+*head);
    printf("%d : %u", *val, *val);
    printf("\n");
    *head += 4;
}
void introspectScanLong(int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memdev+*head);
    printf("%lu", *val);
    printf("\n");
    *head += 8;
}
void introspectScanAddr(int* head, char* name)
{
    printf("%s: ", name);
    uint64_t* val = (uint64_t*)((char*)memdev+*head);
    printf("%p : %016X", *val, *val);
    printf("\n");
    *head += 8;
}

void introspectScanMaybeChar(int* head, int size, char* name)
{
    printf("%s: ", name);
    for(int i=0; i<size; i++)
    {
        uint8_t thisChar = ((char*)memdev+*head)[i];
        if( 0 < thisChar && thisChar < 128 )
        {
            printf("%c",((char*)memdev+*head)[i]);
        }
        else
        {
            printf("%02X",((char*)memdev+*head)[i]);
        }
    }
    printf("\n");
    *head += size;
}

