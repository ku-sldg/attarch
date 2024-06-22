
/* This function looks at the initial ramdisk. */
/* It uses some CAmkES functions to do so. */
/* It does not interact with the Linux VM. */
/* It does not reflect the dynamic state of the Linux VM */

EvidenceBundle* MeasureCAmkESInitRD(uint8_t* memory_device, int* count)
{
    const char name[56] = "InitialRamDisk";
    uint8_t (*digest)[DIGEST_NUM_BYTES] = calloc(1, DIGEST_NUM_BYTES);
    int fd = fs_open("linux-initrd", 0);
    int sizeme = fs_get_size();
    int size = fs_read(fd, sizeme);
    HashMeasure((uint8_t*)fs_get_buf(), 0, sizeme, digest);
    EvidenceBundle* bundle = AllocBundle(&TASK_TYPE, name, digest);
    free(digest);
    *count = 1;
    return bundle;
}

