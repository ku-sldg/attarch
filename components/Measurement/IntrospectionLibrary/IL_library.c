/*
 * An introspection library for Linux 4.9.307
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#define RAM_BASE 0x40000000

// TODO collect these addresses as part of the build process
#define LIST_HEAD_ADDR 0xFB61E0
#define INIT_TASK_ADDR 0xF92280

#include "../hash.h"
#include "IL_utils.c"
#include "IL_kernel_modules.c"
#include "IL_task.c"
