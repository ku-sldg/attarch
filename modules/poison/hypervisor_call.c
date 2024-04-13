#define U64_STRING_SIZE 21

/* Writes the value value to the location location */
/* Namely, value is a pointer (an address), */
/*     and location is the physical address to which value will be written */
void HypervisorAssignPointer(uint64_t location, uint64_t value)
{
    /* dataport write */
    /* signal wait */
    char location_str[U64_STRING_SIZE];
    char value_str[U64_STRING_SIZE];

    // Convert uint64_t values to string
    snprintf(location_str, U64_STRING_SIZE, "%llu", location);
    snprintf(value_str, U64_STRING_SIZE, "%llu", value);

    // Prepare argv array
    char *argv[] = {
        "/usr/bin/dataportAPI",
        "-w",
        location_str,
        value_str,
        NULL
    };

    // Environment array
    char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL
    };

    printk("Starting call_usermodehelper\n");
    int result = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    printk("all_usermodehelper returned %d\n", result);
}

