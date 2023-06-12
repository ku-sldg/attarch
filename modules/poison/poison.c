#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/io.h>
#include <asm/memory.h>

static int __init poison_init(void)
{
    printk("\n==============================\n");
    printk("Poison Module Init");
    printk("\n==============================\n");
    printk("page offset is %llx \n", PAGE_OFFSET);
    /* printk("page END is %llx \n", PAGE_END); */
    printk("kimage_vaddr is %llx \n", KIMAGE_VADDR);
    printk("VA_BITS is %llx \n", VA_BITS);
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

