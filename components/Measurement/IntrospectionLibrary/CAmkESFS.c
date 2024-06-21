
/* This function looks at the initial ramdisk. */
/* It uses some CAmkES functions to do so. */
/* It does not interact with the Linux VM. */
/* It does not reflect the dynamic state of the Linux VM */
/* It is not clear if it is useful. */

void CheckFS()
{
    /* int fd = fs_open("linux-initrd", 0); */
    /* int sizeme = fs_get_size(); */
    /* printf("size of fs is %d\n", sizeme); */
    /* int size = fs_read(fd, 1024); */
    /* for(int i=0; i<4096; i++) */
    /* { */
    /*     if(i % 8 == 0) */
    /*     { */
    /*         printf("\t"); */
    /*     } */
    /*     if(i % 64 == 0) */
    /*     { */
    /*         printf("\n"); */
    /*     } */
    /*     printf("%02X", ((char*)fs_get_buf())[i]); */
    /* } */
}

