#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/err.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <linux/mman.h>

/* #include <string.h> */
/* #include <stdint.h> */
/* #include <sys/mman.h> */
/* #include <sys/types.h> */
/* #include <sys/stat.h> */
/* #include <fcntl.h> */
/* #include <unistd.h> */
/* #include <stdio.h> */

static void printDefinition(char* name, int offset)
{
    printk("#define %s %d\n", name, offset);
}

static int __init poison_init(void)
{
    printk("\n==============================\n");
    printk("Poison Module Init");
    printk("\n==============================\n");
    printk("page offset is %llx \n", PAGE_OFFSET);
    printk("kimage_vaddr is %llx \n", KIMAGE_VADDR);
    printk("VA_BITS is %llx \n", VA_BITS);

    printDefinition("MM_OFFSET", offsetof(struct task_struct, mm));
    printDefinition("PID_OFFSET", offsetof(struct task_struct, pid));
    printDefinition("PARENT_OFFSET", offsetof(struct task_struct, parent));
    printDefinition("CHILDREN_OFFSET", offsetof(struct task_struct, children));
    printDefinition("SIBLING_OFFSET", offsetof(struct task_struct, sibling));
    printDefinition("REAL_CRED_OFFSET", offsetof(struct task_struct, real_cred));
    printDefinition("PGD_OFFSET", offsetof(struct mm_struct, pgd));

    return 0;
}

static void __exit poison_exit(void) {
    printk("\n==============================\n");
    printk("Poison Module Exit");
    printk("\n==============================\n");
}

module_init(poison_init);
module_exit(poison_exit);


MODULE_LICENSE("GPL");

