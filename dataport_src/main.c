#include "dataport_interface.h"

#include <stdio.h>
#include <stdlib.h> // For strtoull
#include <errno.h>  // For errno
#include <limits.h>

int main(int argc, char *argv[]) {
    printf("\n\nTop of DataportAPI\n\n");
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -w <location> <value>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr; // Used by strtoull to check where the number parsing ended
    errno = 0; // To detect any error during conversion

    // Convert the location string to a uint64_t value
    uint64_t location = strtoull(argv[2], &endptr, 10);
    if ((errno == ERANGE && (location == ULLONG_MAX)) || (errno != 0 && location == 0)) {
        perror("strtoull");
        return EXIT_FAILURE;
    }
    if (endptr == argv[2]) {
        fprintf(stderr, "No digits were found\n");
        return EXIT_FAILURE;
    }

    // Convert the value string to a uint64_t value
    uint64_t value = strtoull(argv[3], &endptr, 10);
    if ((errno == ERANGE && (value == ULLONG_MAX)) || (errno != 0 && value == 0)) {
        perror("strtoull");
        return EXIT_FAILURE;
    }
    if (endptr == argv[3]) {
        fprintf(stderr, "No digits were found\n");
        return EXIT_FAILURE;
    }

    // Now location and value are uint64_t numbers, and you can use them as needed
    printf("Location: %llu, Value: %llu\n", location, value);

    void *msg = calloc(1, 4096);
    ((uint64_t*)msg)[0] = 0x1;
    ((uint64_t*)msg)[1] = location;
    ((uint64_t*)msg)[2] = value;


    dpResult output;
    output = write_dataport("/dev/uio0", msg);
    output = emit_dataport("/dev/uio0");
    /* output = wait_dataport("/dev/uio0"); */
    HandleDataportResult(output);

    return EXIT_SUCCESS;
}

