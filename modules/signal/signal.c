/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* Linux kernel module for cross vm connection.
 * This allows the parsing of cross vm connections advertised as a PCI device.
 * Cross vm connections being a combination of events and shared memory regions
 * between linux processes and camkes components). This module utilises userspace io
 * to allow linux processes to mmap the pci bars.
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/uio_driver.h>
#include <asm/io.h>
#include <linux/proc_fs.h>

/* This is an 'abitrary' custom device id we use to match on our dataports
 * pci driver */
#define PCI_CONNECTOR_DEVICE_ID 0xa111
#define MAX_CONNECTOR_DEVICES 32

#define DEVICE_NAME_REGISTER_OFFSET 2
#define DEVICE_NAME_MAX_LEN 50
typedef struct connector_dev_node {
    struct pci_dev *dev;
    struct uio_info *uio;
    char dev_name[DEVICE_NAME_MAX_LEN];
} connector_dev_node_t;

connector_dev_node_t *devices[MAX_CONNECTOR_DEVICES];
unsigned int current_free_dev = 0;

static void send_ready_signal(void)
{
    /*
    printk("beep beep\n");
    connector_dev_node_t* thisDevice = devices[0];
    printk("beepums\n");
    struct uio_info* thisUioInfo = thisDevice->uio;
    printk("beeper\n");
    struct uio_mem thisUioMem = thisUioInfo->mem[0];
    printk("beebeep\n");
    uint32_t* thisEventBar = thisUioMem.internal_addr;
    printk("beepo\n");
    writel(1, &thisEventBar[0]);
    printk("tingo\n");
    */
    uint32_t* event_bar = devices[0]->uio->mem[0].internal_addr;
    writel(1, &event_bar[0]);
}
void workq_fn(struct work_struct*);
DECLARE_WORK(workq,workq_fn);
void workq_fn(struct work_struct* work)
{
    send_ready_signal();
}
ssize_t sched_workq(struct file* buf, char* start, size_t offset, loff_t* len)
{
    printk(KERN_INFO "In proc sched workq");
    schedule_work(&workq);
    return 0;
}
int sched_workq_old(char *buf, char** start, off_t offset, int len, int* eof, void* arg)
{
    printk(KERN_INFO "In proc sched workq");
    schedule_work(&workq);
    return 0;
}
static const struct file_operations proc_file_fops = {
    .owner = THIS_MODULE,
    //.open = open_callback,
    .read = sched_workq,
};







static irqreturn_t connector_event_handler(int irq, struct uio_info *dev_info)
{
    uint32_t *event_bar = dev_info->mem[0].internal_addr;
    u32 val;
    val = readl(&event_bar[1]);
    if (val == 0) {
        /* No event - IRQ wasn't for us */
        return IRQ_NONE;
    }
    /* Clear the register and return IRQ
     * TODO: Currently avoiding IRQ count value - we might want to use it? */
    writel(0, &event_bar[1]);
    return IRQ_HANDLED;
}

static int connector_pci_probe(struct pci_dev *dev,
                               const struct pci_device_id *id)
{
	
    connector_dev_node_t *connector;
    struct uio_info *uio;
    uint32_t *event_bar;
    int num_bars;
    int err = 0;
    int unmap = 0;
    int i = 0;

    if (current_free_dev >= MAX_CONNECTOR_DEVICES) {
        printk("Failed to initialize connector: Managing maximum number of devices\n");
        return -ENODEV;
    }

    connector = kzalloc(sizeof(connector_dev_node_t), GFP_KERNEL);
    if (!connector) {
        printk("Failed to initalize connector\n");
        return -ENOMEM;
    }

    uio = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
    if (!uio) {
        printk("Failed to initialize uio information\n");
        kfree(connector);
        return -ENOMEM;
    }

    if (pci_enable_device(dev)) {
        goto free_alloc;
    }

    if (pci_request_regions(dev, "connector")) {
        goto disable_pci;
    }

    /* Set up the event bar - We assume the event bar is always BAR0
     * even if the PCI device does not use events */
    uio->mem[0].addr = pci_resource_start(dev, 0);
    if (!uio->mem[0].addr) {
        goto disable_pci;
    }
    uio->mem[0].size = pci_resource_len(dev, 0);
    uio->mem[0].internal_addr = pci_ioremap_bar(dev, 0);
    if (!uio->mem[0].internal_addr) {
        goto disable_pci;
    }
    uio->mem[0].memtype = UIO_MEM_PHYS;
    printk("Event Bar (dev-%d) initalised\n", current_free_dev);

    for (i = 1; i < MAX_UIO_MAPS; i++) {
        uio->mem[i].addr = pci_resource_start(dev, i);
        if (!uio->mem[i].addr) {
            /* We assume the first NULL bar is the end
             * Implying that all dataports are passed sequentially (i.e. no gaps) */
            break;
        }

        uio->mem[i].internal_addr = ioremap_cache(pci_resource_start(dev, i),
                                                  pci_resource_len(dev, i));
        if (!uio->mem[i].internal_addr) {
            err = 1;
            break;
        }
        uio->mem[i].size = pci_resource_len(dev, i);
        uio->mem[i].memtype = UIO_MEM_PHYS;
    }
    num_bars = i;
    if (err) {
        goto unmap_bars;
    }

    /* Register our IRQ handler */
    uio->irq = dev->irq;
    uio->irq_flags = IRQF_SHARED;
    uio->handler = connector_event_handler;

    connector->uio = uio;
    connector->dev = dev;
    event_bar = uio->mem[0].internal_addr;
    strncpy(connector->dev_name, (char *)&event_bar[DEVICE_NAME_REGISTER_OFFSET], DEVICE_NAME_MAX_LEN);
    uio->name = connector->dev_name;
    uio->version = "0.0.1";

    if (uio_register_device(&dev->dev, uio)) {
        goto unmap_bars;
    }

    printk("%d Dataports (dev-%d) initalised\n", num_bars, current_free_dev);
    pci_set_drvdata(dev, uio);
    devices[current_free_dev++] = connector;

	
    printk("==============================\n");
    printk("Signal Module Init\n");
    printk("Creating /proc/sched_workq...\n");
    proc_create("sched_workq", 0, NULL, &proc_file_fops);
    printk("Try `cat` on it to signal for a measurement.\n");
    /*
    void* entryAddr0 = (void*)0xFFFF000008FB61E0;
    void* entryAddr1 = (void*)0xFFFF0000007C2188;
    void* entryAddr2 = (void*)0xFFFF0000007CA008;
    printk("You input vaddr: %p\n", entryAddr0);
    printk("I give back paddr: %p\n", virt_to_phys(entryAddr0));
    printk("You input vaddr: %p\n", entryAddr1);
    printk("I give back paddr: %p\n", virt_to_phys(entryAddr1));
    printk("You input vaddr: %p\n", entryAddr2);
    printk("I give back paddr: %p\n", virt_to_phys(entryAddr2));
    printk("==============================\n");
    void* entryAddr3 = (void*)0xFFFF00000913D000;
    printk("You input vaddr: %p\n", entryAddr3);
    printk("I give back paddr: %p\n", virt_to_phys(entryAddr3));
    void* entryAddr4 = (void*)0xFFFF000047FFF803;
    printk("You input vaddr: %p\n", entryAddr4);
    printk("I give back paddr: %p\n", virt_to_phys(entryAddr4));
    printk("==============================\n");
    printk("Sizeof pmd_t is %X\n", sizeof(pmd_t));
    printk("Sizeof pte_t is %X\n", sizeof(pte_t));
    printk("==============================\n");
    printk("CONFIG_PGTABLE_LEVELS is %d\n", CONFIG_PGTABLE_LEVELS);
    printk("page_mask is %X\n", PAGE_MASK);
    printk("pmd_mask is %X\n", PMD_MASK);
    printk("pud_mask is %X\n", PUD_MASK);
    printk("pgdir_mask is %X\n", PGDIR_MASK);
    printk("page_SHIFT is %d\n", PAGE_SHIFT);
    printk("pmd_SHIFT is %d\n", PMD_SHIFT);
    printk("pud_SHIFT is %d\n", PUD_SHIFT);
    printk("pgdir_SHIFT is %d\n", PGDIR_SHIFT);
    printk("==============================\n");
    printk("Module_name_len is %d\n", MODULE_NAME_LEN);
    printk("==============================\n");
    */



    /*
    printk("enum module_state size is %d\n", sizeof(enum module_state));
    printk("struct list_head size is %d\n", sizeof(struct list_head));
    printk("struct module_kobject size is %d\n", sizeof(struct module_kobject));
    printk("struct module_attribute size is %d\n", sizeof(struct module_attribute));
    printk("const char* size is %d\n", sizeof(const char*));
    printk("struct kobject size is %d\n", sizeof(struct kobject));
    printk("const struct kernel_symbol size is %d\n", sizeof(const struct kernel_symbol));
    printk("const unsigned long* size is %d\n", sizeof(const unsigned long*));
    printk("unsigned int size is %d\n", sizeof(unsigned int));
    printk("struct mutex size is %d\n", sizeof(struct mutex));
    printk("struct kernel_param size is %d\n", sizeof(struct kernel_param));
    printk("bool size is %d\n", sizeof(bool));
    printk("struct exception_table_entry* size is %d\n", sizeof(struct exception_table_entry*));
    printk("char* size is %d\n", sizeof(char*));
    printk("struct module_layout size is %d\n", sizeof(struct module_layout));
    */

    return 0;
unmap_bars:
    iounmap(uio->mem[0].internal_addr);
    for (unmap = 1; unmap < num_bars; unmap++) {
        iounmap(uio->mem[unmap].internal_addr);
    }
    pci_release_regions(dev);
disable_pci:
    pci_disable_device(dev);
free_alloc:
    kfree(uio);
    kfree(connector);
    return -ENODEV;
}

static void connector_pci_remove(struct pci_dev *dev)
{
    remove_proc_entry("sched_workq", NULL);
    printk("\n==============================\n");
    printk("Signal Module Exit");
    printk("\n==============================\n");
	
    struct uio_info *uio = pci_get_drvdata(dev);
    uio_unregister_device(uio);
    pci_release_regions(dev);
    pci_disable_device(dev);
    kfree(uio);
}

static struct pci_device_id connector_pci_ids[] = {
    {
        .vendor =       PCI_VENDOR_ID_REDHAT_QUMRANET,
        .device =       PCI_CONNECTOR_DEVICE_ID,
        .subvendor =    PCI_ANY_ID,
        .subdevice =    PCI_ANY_ID,
    },
    {0,}
};

static struct pci_driver connector_pci_driver = {
    .name = "connector",
    .id_table = connector_pci_ids,
    .probe = connector_pci_probe,
    .remove = connector_pci_remove,
};

module_pci_driver(connector_pci_driver);
MODULE_DEVICE_TABLE(pci, connector_pci_ids);
MODULE_LICENSE("GPL v2");
