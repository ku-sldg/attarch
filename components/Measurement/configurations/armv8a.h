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

// found in [linux kernel]/include/linux/sched.h and common apparently to ALL linux kernels
#define TASK_COMM_LEN 16

// TODO extract these automatically from system.map
#define LIST_HEAD_ADDR 0xFB61E0

// derived Linux level definitions
#define NUM_RODATA_PAGES (INTRO___END_RODATA_VADDR-INTRO___START_RODATA_VADDR)/INTRO_PAGE_SIZE

// introspection level definitions
#define INTRO_NUM_DIGESTS 100 //arbitrary
#define DIGEST_NUM_BYTES 64 // representative of 512 bit hash digests

// task specific definitions
#define NUM_TASKS 20 //arbitrary
#define NUM_CHILD_TASKS 64 //arbitrary

// kernel module specific definitions
#define NUM_MODULE_DIGESTS 128 //arbitrary

#endif

