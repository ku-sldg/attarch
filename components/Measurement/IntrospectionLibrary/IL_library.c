/*
 * An introspection library for Linux 4.9.307
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#define RAM_BASE 0x40000000
#define RAM_SIZE 0x8001000
#define NUM_TASKS 20

// TODO collect these addresses as part of the build process
#define LIST_HEAD_ADDR 0xFB61E0
#define INIT_TASK_ADDR 0xF92280

#include "../hash.h"
#include "../KnownDigests.h"
#include "IL_utils.c"
#include "IL_kernel_modules.c"
#include "IL_task.c"
#include "IL_kernel_rodata.c"
