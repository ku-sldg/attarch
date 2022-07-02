#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/io.h>

//includes for emit/consume events
//#include <stdio.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <assert.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/mman.h>


struct buffer {
    char* p;
    int pos;
    int size;
};

static int __init poison_init(void)
{
    printk("\n==============================\n");
    printk("Poison Module Init");
    printk("\n==============================\n");
    return 0;
}

static void __exit poison_exit(void) {
    printk("\n==============================\n");
    printk("Poison Module Exit");
    printk("\n==============================\n");
}

module_init(poison_init);
module_exit(poison_exit);



