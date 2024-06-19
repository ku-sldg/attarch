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

    printDefinition("SUPER_BLOCK_S_TYPE", offsetof(struct super_block, s_type));
    printDefinition("SUPER_BLOCK_S_ROOT", offsetof(struct super_block, s_root));
    printDefinition("DENTRY_D_INODE", offsetof(struct dentry, d_inode));
    printDefinition("DENTRY_D_NAME", offsetof(struct dentry, d_name));
    printDefinition("DENTRY_D_SUBDIRS", offsetof(struct dentry, d_subdirs));
    printDefinition("DENTRY_D_CHILD", offsetof(struct dentry, d_child));
    printDefinition("INODE_I_DATA", offsetof(struct inode, i_data));
    printDefinition("ADDRESS_SPACE_I_PAGES", offsetof(struct address_space, i_pages));

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

