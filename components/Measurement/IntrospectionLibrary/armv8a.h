/* Previously registered sha512 digests
 * includes: task rodata, kernel module rodata
 * Michael Neises
 * 11 July 2022
 */

#ifndef INTRO_CONFIG_H
#define INTRO_CONFIG_H_

#include "linux_definitions.h" //generated from ScrapeSystemMap.py

// camkes level definitions
#define RAM_BASE 0x40000000
#define RAM_SIZE 0x8001000

// found in [linux kernel]/include/linux/sched.h and common apparently to ALL linux kernels
#define TASK_COMM_LEN 16

// derived Linux level definitions
#define NUM_RODATA_PAGES (INTRO_END_RODATA_VADDR-INTRO_START_RODATA_VADDR)/INTRO_PAGE_SIZE
#define NUM_HYP_RODATA_PAGES (INTRO_HYP_RODATA_END_VADDR-INTRO_HYP_RODATA_START_VADDR)/INTRO_PAGE_SIZE
#define NUM_TEXT_PAGES (INTRO_ETEXT_VADDR-INTRO_TEXT_VADDR)/INTRO_PAGE_SIZE
#define NUM_TRAMP_ENTRY_TEXT_PAGES (INTRO_ENTRY_TRAMP_TEXT_END_VADDR-INTRO_ENTRY_TRAMP_TEXT_START_VADDR)/INTRO_PAGE_SIZE
//#define NUM_HIBERNATE_EXIT_TEXT_PAGES (INTRO_HIBERNATE_EXIT_TEXT_END_VADDR-INTRO_HIBERNATE_EXIT_TEXT_START_VADDR)/INTRO_PAGE_SIZE
// these are both less than one page, so we measure the exact bytes instead
//#define NUM_RELOCATE_NEW_KERNEL_PAGES (INTRO_RELOCATE_NEW_KERNEL_END_VADDR-INTRO_RELOCATE_NEW_KERNEL_START_VADDR)/INTRO_PAGE_SIZE
#define NUM_INIT_PGD_PAGES (INTRO_END_VADDR-INTRO_INIT_PG_DIR_VADDR)/INTRO_PAGE_SIZE

// introspection level definitions
#define INTRO_NUM_DIGESTS 100 //arbitrary
#define DIGEST_NUM_BYTES 64 // representative of 512 bit hash digests

// task specific definitions
#define NUM_TASKS 100 //arbitrary
#define NUM_CHILD_TASKS 64 //arbitrary

// kernel module specific definitions
#define NUM_MODULE_DIGESTS 128 //arbitrary

#endif

