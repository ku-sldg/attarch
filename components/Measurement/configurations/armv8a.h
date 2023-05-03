/* Previously registered sha512 digests
 * includes: task rodata, kernel module rodata
 * Michael Neises
 * 11 July 2022
 */

#ifndef INTRO_CONFIG_H
#define INTRO_CONFIG_H_

// camkes level definitions
#define RAM_BASE 0x40000000
#define RAM_SIZE 0x8001000

// linux level definitions
// TODO extract these automatically from system.map
//#define PAGE_SIZE 0x1000 // PAGE_SIZE is available from musllibc/build-temp/stage/include/bits/limits.h
#define LIST_HEAD_ADDR 0xFB61E0
#define INIT_TASK_ADDR 0xF92280
#define START_RODATA 0xffff0000087c0000
#define END_RODATA 0xffff000008bbf000
#define START_READ_ONLY_AFTER_INIT 0xffff00000899d7dc
#define END_READ_ONLY_AFTER_INIT 0xffff00000899e160

// introspection level definitions
#define INTRO_NUM_DIGESTS 100
#define DIGEST_NUM_BYTES 64

// task specific definitions
#define NUM_TASKS 20
#define NUM_CHILD_TASKS 64

// kernel module specific definitions
#define NUM_MODULE_DIGESTS 128
#define MODULE_NAME_LEN 56

#endif

