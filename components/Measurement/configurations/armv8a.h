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

// It appears we can scrape PAGE_SIZE via a PAGE_SHIFT option in linux/.config
#define INTRO_PAGE_SIZE 0x1000

// found in [linux kernel]/include/linux/sched.h and common apparently to ALL linux kernels
#define TASK_COMM_LEN 16

// TODO extract these automatically from system.map
#define LIST_HEAD_ADDR 0xFB61E0
#define INIT_TASK_ADDR 0xF92280
#define START_RODATA 0xffff0000087c0000
#define END_RODATA 0xffff000008bbf000
#define START_READ_ONLY_AFTER_INIT 0xffff00000899d7dc
#define END_READ_ONLY_AFTER_INIT 0xffff00000899e160

// derived Linux level definitions
#define NUM_RODATA_PAGES (END_RODATA-START_RODATA)/INTRO_PAGE_SIZE

// introspection level definitions
#define INTRO_NUM_DIGESTS 100 //arbitrary
#define DIGEST_NUM_BYTES 64 // representative of 512 bit hash digests

// task specific definitions
#define NUM_TASKS 20 //arbitrary
#define NUM_CHILD_TASKS 64 //arbitrary

// kernel module specific definitions
#define NUM_MODULE_DIGESTS 128 //arbitrary

#endif

