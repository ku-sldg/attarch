/*
 * An introspection library for Linux 4.9.307
 * Michael Neises
 * 11 July 2022
 */

#include <string.h>
#include <camkes.h>
#include <stdarg.h>

#include "../hash.h"
#include "../KnownDigests.h"
#include "../configurations/include.h"
#include "IL_utils.c"
#include "IL_kernel_modules.c"
#include "IL_task.c"
#include "IL_kernel_rodata.c"
