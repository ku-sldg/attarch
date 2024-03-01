#include "dataport_interface.h"

void HandleDataportResult(dpResult err)
{
    if(err == NULL_INPUT)
    {
        printf("An input was wrongly NULL.\n");
    }
    else if(err == FILE_NOT_FOUND)
    {
        printf("Failed to find that File.\n");
    }
    else if(err == FILE_NOT_MAPPED)
    {
        printf("Failed to map that File.\n");
    }
    else if(err == FILE_READ_FAILURE)
    {
        printf("Failed to read that File.\n");
    }
}
dpResult writeAction(PagePointer dp, PagePointer msg)
{
    printf("writing\n");
    if(dp == NULL)
    {
        printf(" dp null \n ");
        return NULL_INPUT;
    }
    if(msg == NULL)
    {
        printf(" msg null \n ");
        return NULL_INPUT;
    }
    memcpy((void*)dp, (void*)msg, DP_PAGE_SIZE);
    return SUCCESS;
}
dpResult readAction(PagePointer dp, PagePointer msg)
{
    printf("reading\n");
    memcpy((void*)msg, (void*)dp, DP_PAGE_SIZE);
    return SUCCESS;
}
dpResult waitAction(int fd)
{
    printf("waiting\n");
    dpResult output;
    int val;
    int result = read(fd, &val, sizeof(int));
    if (result != sizeof(int)) {
        return FILE_READ_FAILURE;
    }
    // don't think this is necessary for the api,
    // rather it is an artifact of the cakeml ffi
    // what is val, I wonder?
    /* memcpy((void *)a+1, (const void *)(&val), sizeof(int)); */
    return SUCCESS;
}
dpResult emitAction(int fd)
{
    printf("emitting\n");
    dpResult output;
    void * dataport = mmap(NULL, 1, PROT_WRITE, MAP_SHARED, fd, 0);
    if (dataport == (void *)(-1))
    {
        return FILE_NOT_MAPPED;
    }
    *((uint8_t *)dataport) = 1;
    munmap(dataport, DP_PAGE_SIZE);
    return SUCCESS;
}
dpResult DoWithDataport(int fd, int flags, DataportAction dpJob, PagePointer msg)
{
    if(dpJob == NULL || msg == NULL)
    {
        return NULL_INPUT;
    }
    dpResult output;
    void * dataport = mmap(NULL, DP_PAGE_SIZE, flags, MAP_SHARED, fd, getpagesize());
    if (dataport == (void *)(-1))
    {
        return FILE_NOT_MAPPED;
    }
    output = dpJob(dataport, msg);
    munmap(dataport, DP_PAGE_SIZE);
    return output;
}
dpResult DoWithFile(const char* name, int file_flags, int dataport_flags, PagePointer msg, FileAction fileJob, DataportAction dpJob)
{
    dpResult output;
    int fd = open(name, file_flags);
    if (fd == -1)
    {
        return FILE_NOT_FOUND;
    }
    if(dpJob)
    {
        if(msg)
        {
            output = DoWithDataport(fd, dataport_flags, dpJob, msg);
        }
        else
        {
            output = NULL_INPUT;
        }
    }
    else
    {
        if(fileJob)
        {
            output = fileJob(fd);
        }
        else
        {
            output = NULL_INPUT;
        }
    }
    close(fd);
    return output;
}
dpResult write_dataport(const char *name, PagePointer msg)
{
    return DoWithFile(name, O_RDWR, PROT_WRITE, msg, NULL, writeAction);
}
dpResult read_dataport(const char *name, PagePointer msg)
{
    return DoWithFile(name, O_RDONLY, PROT_READ, msg, NULL, readAction);
}
dpResult emit_dataport(const char *name)
{
    return DoWithFile(name, O_RDWR, PROT_WRITE, NULL, emitAction, NULL);
}
dpResult wait_dataport(const char *name)
{
    return DoWithFile(name, O_RDONLY, PROT_READ, NULL, waitAction, NULL);
}

/* void WaitAction() */
/* { */
/*     int val; */
/*     int result = read(fd, &val, sizeof(int)); */
/*     memcpy((void *)a+1, (const void *)(&val), sizeof(int)); */
/* } */
/* void ffiemitDataport(const uint8_t * c, const long clen, uint8_t * a, const long alen) { */
/*     int fd = open(name, O_RDWR); */
/*     void * dataport = mmap(NULL, 1, PROT_WRITE, MAP_SHARED, fd, 0); */
/*     *((uint8_t *)dataport) = 1; */
/* } */

