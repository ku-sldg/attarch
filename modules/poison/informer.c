void inform()
{
    void* init_task_addr = (void*)0xFFFF000008F92280;
    printk("you give: %p\n", init_task_addr);
    printk("i give: %p\n", virt_to_phys(init_task_addr));

#if defined(CONFIG_SLOB)
    printk("CONFIG_SLOB is defined\n");
#else
    printk("CONFIG_SLOB is not defined\n");
#endif
#if defined(CONFIG_THREAD_INFO_IN_TASK)
    printk("CONFIG_THREAD_INFO_IN_TASK is defined\n");
#else
    printk("CONFIG_THREAD_INFO_IN_TASK is not defined\n");
#endif
#if defined(CONFIG_SMP)
    printk("CONFIG_SMP is defined\n");
#else
    printk("CONFIG_SMP is not defined\n");
#endif
#if defined(CONFIG_THREAD_INFO_IN_TASK)
    printk("CONFIG_THREAD_INFO_IN_TASK is defined\n");
#else
    printk("CONFIG_THREAD_INFO_IN_TASK is not defined\n");
#endif
#if defined(CONFIG_CGROUP_SCHED)
    printk("CONFIG_CGROUP_SCHED is defined\n");
#else
    printk("CONFIG_CGROUP_SCHED is not defined\n");
#endif
#if defined(CONFIG_PREEMPT_NOTIFIERS)
    printk("CONFIG_PREEMPT_NOTIFIERS is defined\n");
#else
    printk("CONFIG_PREEMPT_NOTIFIERS is not defined\n");
#endif
#if defined(CONFIG_BLK_DEV_IO_TRACE)
    printk("CONFIG_BLK_DEV_IO_TRACE is defined\n");
#else
    printk("CONFIG_BLK_DEV_IO_TRACE is not defined\n");
#endif
#if defined(CONFIG_PREEMPT_RCU)
    printk("CONFIG_PREEMPT_RCU is defined\n");
#else
    printk("CONFIG_PREEMPT_RCU is not defined\n");
#endif
#if defined(CONFIG_TASKS_RCU)
    printk("CONFIG_TASKS_RCU is defined\n");
#else
    printk("CONFIG_TASKS_RCU is not defined\n");
#endif
#if defined(CONFIG_SCHED_INFO)
    printk("CONFIG_SCHED_INFO is defined\n");
#else
    printk("CONFIG_SCHED_INFO is not defined\n");
#endif
#if defined(CONFIG_SMP)
    printk("CONFIG_SMP is defined\n");
#else
    printk("CONFIG_SMP is not defined\n");
#endif
#if defined(CONFIG_MEMCG)
    printk("CONFIG_MEMCG is defined\n");
#else
    printk("CONFIG_MEMCG is not defined\n");
#endif
#if defined(CONFIG_COMPAT_BRK)
    printk("CONFIG_COMPAT_BRK is defined\n");
#else
    printk("CONFIG_COMPAT_BRK is not defined\n");
#endif
#if defined(CONFIG_CGROUPS)
    printk("CONFIG_CGROUPS is defined\n");
#else
    printk("CONFIG_CGROUPS is not defined\n");
#endif
#if defined(CONFIG_CC_STACKPROTECTOR)
    printk("CONFIG_CC_STACKPROTECTOR is defined\n");
#else
    printk("CONFIG_CC_STACKPROTECTOR is not defined\n");
#endif
#if defined(CONFIG_VIRT_CPU_ACCOUNTING_GEN)
    printk("CONFIG_VIRT_CPU_ACCOUNTING_GEN is defined\n");
#else
    printk("CONFIG_VIRT_CPU_ACCOUNTING_GEN is not defined\n");
#endif
#if defined(CONFIG_NO_HZ_FULL)
    printk("CONFIG_NO_HZ_FULL is defined\n");
#else
    printk("CONFIG_NO_HZ_FULL is not defined\n");
#endif
#if defined(CONFIG_SYSVIPC)
    printk("CONFIG_SYSVIPC is defined\n");
#else
    printk("CONFIG_SYSVIPC is not defined\n");
#endif
#if defined(CONFIG_DETECT_HUNG_TASK)
    printk("CONFIG_DETECT_HUNG_TASK is defined\n");
#else
    printk("CONFIG_DETECT_HUNG_TASK is not defined\n");
#endif
#if defined(CONFIG_AUDITSYSCALL)
    printk("CONFIG_AUDITSYSCALL is defined\n");
#else
    printk("CONFIG_AUDITSYSCALL is not defined\n");
#endif
#if defined(CONFIG_RT_MUTEXES)
    printk("CONFIG_RT_MUTEXES is defined\n");
#else
    printk("CONFIG_RT_MUTEXES is not defined\n");
#endif
#if defined(CONFIG_DEBUG_MUTEXES)
    printk("CONFIG_DEBUG_MUTEXES is defined\n");
#else
    printk("CONFIG_DEBUG_MUTEXES is not defined\n");
#endif
#if defined(CONFIG_TRACE_IRQFLAGS)
    printk("CONFIG_TRACE_IRQFLAGS is defined\n");
#else
    printk("CONFIG_TRACE_IRQFLAGS is not defined\n");
#endif
#if defined(CONFIG_LOCKDEP)
    printk("CONFIG_LOCKDEP is defined\n");
#else
    printk("CONFIG_LOCKDEP is not defined\n");
#endif
#if defined(CONFIG_UBSAN)
    printk("CONFIG_UBSAN is defined\n");
#else
    printk("CONFIG_UBSAN is not defined\n");
#endif
#if defined(CONFIG_BLOCK)
    printk("CONFIG_BLOCK is defined\n");
#else
    printk("CONFIG_BLOCK is not defined\n");
#endif
#if defined(CONFIG_CPUSETS)
    printk("CONFIG_CPUSETS is defined\n");
#else
    printk("CONFIG_CPUSETS is not defined\n");
#endif
#if defined(CONFIG_CGROUPS)
    printk("CONFIG_CGROUPS is defined\n");
#else
    printk("CONFIG_CGROUPS is not defined\n");
#endif
#if defined(CONFIG_FUTEX)
    printk("CONFIG_FUTEX is defined\n");
#else
    printk("CONFIG_FUTEX is not defined\n");
#endif
#if defined(CONFIG_COMPAT)
    printk("CONFIG_COMPAT is defined\n");
#else
    printk("CONFIG_COMPAT is not defined\n");
#endif
#if defined(CONFIG_PERF_EVENTS)
    printk("CONFIG_PERF_EVENTS is defined\n");
#else
    printk("CONFIG_PERF_EVENTS is not defined\n");
#endif
#if defined(CONFIG_DEBUG_PREEMPT)
    printk("CONFIG_DEBUG_PREEMPT is defined\n");
#else
    printk("CONFIG_DEBUG_PREEMPT is not defined\n");
#endif
#if defined(CONFIG_NUMA)
    printk("CONFIG_NUMA is defined\n");
#else
    printk("CONFIG_NUMA is not defined\n");
#endif
#if defined(CONFIG_NUMA_BALANCING)
    printk("CONFIG_NUMA_BALANCING is defined\n");
#else
    printk("CONFIG_NUMA_BALANCING is not defined\n");
#endif
#if defined(CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH)
    printk("CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH is defined\n");
#else
    printk("CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH is not defined\n");
#endif
#if defined(CONFIG_TASK_DELAY_ACCT)
    printk("CONFIG_TASK_DELAY_ACCT is defined\n");
#else
    printk("CONFIG_TASK_DELAY_ACCT is not defined\n");
#endif
#if defined(CONFIG_FAULT_INJECTION)
    printk("CONFIG_FAULT_INJECTION is defined\n");
#else
    printk("CONFIG_FAULT_INJECTION is not defined\n");
#endif
#if defined(CONFIG_LATENCYTOP)
    printk("CONFIG_LATENCYTOP is defined\n");
#else
    printk("CONFIG_LATENCYTOP is not defined\n");
#endif
#if defined(CONFIG_KASAN)
    printk("CONFIG_KASAN is defined\n");
#else
    printk("CONFIG_KASAN is not defined\n");
#endif
#if defined(CONFIG_FUNCTION_GRAPH_TRACER)
    printk("CONFIG_FUNCTION_GRAPH_TRACER is defined\n");
#else
    printk("CONFIG_FUNCTION_GRAPH_TRACER is not defined\n");
#endif
#if defined(CONFIG_TRACING)
    printk("CONFIG_TRACING is defined\n");
#else
    printk("CONFIG_TRACING is not defined\n");
#endif
#if defined(CONFIG_KCOV)
    printk("CONFIG_KCOV is defined\n");
#else
    printk("CONFIG_KCOV is not defined\n");
#endif
#if defined(CONFIG_MEMCG)
    printk("CONFIG_MEMCG is defined\n");
#else
    printk("CONFIG_MEMCG is not defined\n");
#endif
#if defined(CONFIG_UPROBES)
    printk("CONFIG_UPROBES is defined\n");
#else
    printk("CONFIG_UPROBES is not defined\n");
#endif
#if defined(CONFIG_DEBUG_ATOMIC_SLEEP)
    printk("CONFIG_DEBUG_ATOMIC_SLEEP is defined\n");
#else
    printk("CONFIG_DEBUG_ATOMIC_SLEEP is not defined\n");
#endif
#if defined(CONFIG_MMU)
    printk("CONFIG_MMU is defined\n");
#else
    printk("CONFIG_MMU is not defined\n");
#endif
#if defined(CONFIG_VMAP_STACK)
    printk("CONFIG_VMAP_STACK is defined\n");
#else
    printk("CONFIG_VMAP_STACK is not defined\n");
#endif
#if defined(CONFIG_THREAD_INFO_IN_TASK)
    printk("CONFIG_THREAD_INFO_IN_TASK is defined\n");
#else
    printk("CONFIG_THREAD_INFO_IN_TASK is not defined\n");
#endif
#if defined(SPLIT_RSS_COUNTING)
    printk("SPLIT_RSS_COUNTING is defined\n");
#else
    printk("SPLIT_RSS_COUNTING is not defined\n");
#endif
#if defined(TIF_RESTORE_SIGMASK)
    printk("TIF_RESTORE_SIGMASK is defined\n");
#else
    printk("TIF_RESTORE_SIGMASK is not defined\n");
#endif
#if defined(CONFIG_TASK_XACCT)
    printk("CONFIG_TASK_XACCT is defined\n");
#else
    printk("CONFIG_TASK_XACCT is not defined\n");
#endif
#if defined(CONFIG_BCACHE)
    printk("CONFIG_BCACHE is defined\n");
#else
    printk("CONFIG_BCACHE is not defined\n");
#endif
#if defined(CONFIG_BCACHE_MODULE)
    printk("CONFIG_BCACHE_MODULE is defined\n");
#else
    printk("CONFIG_BCACHE_MODULE is not defined\n");
#endif

    printk("VMACACHE_SIZE is %d\n", VMACACHE_SIZE);
    printk("PIDTYPE_MAX is %d\n", PIDTYPE_MAX);
    printk("perf_nr_task_contexts is %d\n", perf_nr_task_contexts);
    printk("TASK_COMM_LEN is %d\n", TASK_COMM_LEN);

    printk("volatile long  state %ld\n", sizeof(volatile long ));	
    printk("void * stack %ld\n", sizeof(void *));
    printk("atomic_t  usage %ld\n", sizeof(atomic_t ));
    printk("unsigned int  flags %ld\n", sizeof(unsigned int ));	
    printk("unsigned int  ptrace %ld\n", sizeof(unsigned int ));
    printk("struct llist_node  wake_entry %ld\n", sizeof(struct llist_node ));
    printk("int  on_cpu %ld\n", sizeof(int ));
    printk("unsigned int  wakee_flips %ld\n", sizeof(unsigned int ));
    printk("unsigned long  wakee_flip_decay_ts %ld\n", sizeof(unsigned long ));
    printk("struct task_struct * last_wakee %ld\n", sizeof(struct task_struct *));
    printk("int  wake_cpu %ld\n", sizeof(int ));
    printk("int  on_rq %ld\n", sizeof(int ));
    printk("int  prio %ld\n", sizeof(int ));
    printk("int  static_prio %ld\n", sizeof(int ));
    printk("int  normal_prio %ld\n", sizeof(int ));
    printk("unsigned int  rt_priority %ld\n", sizeof(unsigned int ));
    printk("const struct sched_class * sched_class %ld\n", sizeof(const struct sched_class *));
    printk("struct sched_entity  se %ld\n", sizeof(struct sched_entity ));
    printk("struct sched_rt_entity  rt %ld\n", sizeof(struct sched_rt_entity ));
    printk("struct task_group * sched_task_group %ld\n", sizeof(struct task_group *));
    printk("struct sched_dl_entity  dl %ld\n", sizeof(struct sched_dl_entity ));
    printk("struct hlist_head  preempt_notifiers %ld\n", sizeof(struct hlist_head ));
    printk("unsigned int  btrace_seq %ld\n", sizeof(unsigned int ));
    printk("unsigned int  policy %ld\n", sizeof(unsigned int ));
    printk("int  nr_cpus_allowed %ld\n", sizeof(int ));
    printk("cpumask_t  cpus_allowed %ld\n", sizeof(cpumask_t ));
    printk("struct sched_info  sched_info %ld\n", sizeof(struct sched_info ));
    printk("struct list_head  tasks %ld\n", sizeof(struct list_head ));
    printk("struct plist_node  pushable_tasks %ld\n", sizeof(struct plist_node ));
    printk("struct rb_node  pushable_dl_tasks %ld\n", sizeof(struct rb_node ));
    printk("struct mm_struct * mm %ld\n", sizeof(struct mm_struct *));
    printk("struct mm_struct * active_mm %ld\n", sizeof(struct mm_struct *));
    printk("u64  vmacache_seqnum %ld\n", sizeof(u64 ));
    printk("struct vm_area_struct * vmacache1 %ld\n", sizeof(struct vm_area_struct *));
    printk("struct vm_area_struct * vmacache2 %ld\n", sizeof(struct vm_area_struct *));
    printk("struct vm_area_struct * vmacache3 %ld\n", sizeof(struct vm_area_struct *));
    printk("struct vm_area_struct * vmacache4 %ld\n", sizeof(struct vm_area_struct *));
    printk("struct  task_rss_stat	rss_stat %ld\n", sizeof(struct task_rss_stat ));
    printk("int  exit_state %ld\n", sizeof(int ));
    printk("int  exit_code %ld\n", sizeof(int ));
    printk("int  exit_signal %ld\n", sizeof(int ));
    printk("int  pdeath_signal %ld\n", sizeof(int ));  
    printk("unsigned long  jobctl %ld\n", sizeof(unsigned long ));	
    printk("unsigned int  personality %ld\n", sizeof(unsigned int ));
    printk("unsigned int  bits1 %ld\n", sizeof(unsigned int ));
    printk("unsigned int  bits2 %ld\n", sizeof(unsigned int ));
    printk("unsigned long  atomic_flags %ld\n", sizeof(unsigned long )); 
    printk("struct restart_block  restart_block %ld\n", sizeof(struct restart_block ));
    printk("pid_t  pid %ld\n", sizeof(pid_t ));
    printk("pid_t  tgid %ld\n", sizeof(pid_t ));
    printk("unsigned long  stack_canary %ld\n", sizeof(unsigned long ));
    printk("struct task_struct __rcu * real_parent %ld\n", sizeof(struct task_struct __rcu *)); 
    printk("struct task_struct __rcu * parent %ld\n", sizeof(struct task_struct __rcu *)); 
    printk("struct list_head  children %ld\n", sizeof(struct list_head ));	
    printk("struct list_head  sibling %ld\n", sizeof(struct list_head ));	
    printk("struct task_struct * group_leader %ld\n", sizeof(struct task_struct *));	
    printk("struct list_head  ptraced %ld\n", sizeof(struct list_head ));
    printk("struct list_head  ptrace_entry %ld\n", sizeof(struct list_head ));
    printk("struct pid_link  pids1 %ld\n", sizeof(struct pid_link ));
    printk("struct pid_link  pids2 %ld\n", sizeof(struct pid_link ));
    printk("struct pid_link  pids3 %ld\n", sizeof(struct pid_link ));
    printk("struct list_head  thread_group %ld\n", sizeof(struct list_head ));
    printk("struct list_head  thread_node %ld\n", sizeof(struct list_head ));
    printk("struct completion * vfork_done %ld\n", sizeof(struct completion *));		
    printk("int __user * set_child_tid %ld\n", sizeof(int __user *));		
    printk("int __user * clear_child_tid %ld\n", sizeof(int __user *));		
    printk("cputime_t  utime %ld\n", sizeof(cputime_t ));
    printk("cputime_t  stime %ld\n", sizeof(cputime_t ));
    printk("cputime_t  utimescaled %ld\n", sizeof(cputime_t ));
    printk("cputime_t  stimescaled %ld\n", sizeof(cputime_t ));
    printk("cputime_t  gtime %ld\n", sizeof(cputime_t ));
    printk("struct prev_cputime  prev_cputime %ld\n", sizeof(struct prev_cputime ));
    printk("unsigned long  nvcsw %ld\n", sizeof(unsigned long )); 
    printk("unsigned long  nivcsw %ld\n", sizeof(unsigned long )); 
    printk("u64  start_time %ld\n", sizeof(u64 ));		
    printk("u64  real_start_time %ld\n", sizeof(u64 ));	
    printk("unsigned long  min_flt %ld\n", sizeof(unsigned long ));
    printk("unsigned long  maj_flt %ld\n", sizeof(unsigned long ));
    printk("struct task_cputime  cputime_expires %ld\n", sizeof(struct task_cputime ));
    printk("struct list_head  cpu_timers1 %ld\n", sizeof(struct list_head ));
    printk("struct list_head  cpu_timers2 %ld\n", sizeof(struct list_head ));
    printk("struct list_head  cpu_timers3 %ld\n", sizeof(struct list_head ));
    printk("const struct cred __rcu * ptracer_cred %ld\n", sizeof(const struct cred __rcu *)); 
    printk("const struct cred __rcu * real_cred %ld\n", sizeof(const struct cred __rcu *));
    printk("const struct cred __rcu * cred %ld\n", sizeof(const struct cred __rcu *));
    printk("char  comm %ld\n", sizeof(char ));
    printk("struct nameidata * nameidata %ld\n", sizeof(struct nameidata *));
    printk("struct sysv_sem  sysvsem %ld\n", sizeof(struct sysv_sem ));
    printk("struct sysv_shm  sysvshm %ld\n", sizeof(struct sysv_shm ));
    printk("unsigned long  last_switch_count %ld\n", sizeof(unsigned long ));
    printk("struct fs_struct * fs %ld\n", sizeof(struct fs_struct *));
    printk("struct files_struct * files %ld\n", sizeof(struct files_struct *));
    printk("struct nsproxy * nsproxy %ld\n", sizeof(struct nsproxy *));
    printk("struct signal_struct * signal %ld\n", sizeof(struct signal_struct *));
    printk("struct sighand_struct * sighand %ld\n", sizeof(struct sighand_struct *));
    printk("sigset_t  blocked %ld\n", sizeof(sigset_t ));
    printk("sigset_t  real_blocked %ld\n", sizeof(sigset_t ));
    printk("sigset_t  saved_sigmask %ld\n", sizeof(sigset_t ));	
    printk("struct sigpending  pending %ld\n", sizeof(struct sigpending ));
    printk("unsigned long  sas_ss_sp %ld\n", sizeof(unsigned long ));
    printk("size_t  sas_ss_size %ld\n", sizeof(size_t ));
    printk("unsigned  sas_ss_flags %ld\n", sizeof(unsigned ));
    printk("struct callback_head * task_works %ld\n", sizeof(struct callback_head *));
    printk("struct audit_context * audit_context %ld\n", sizeof(struct audit_context *));
    printk("kuid_t  loginuid %ld\n", sizeof(kuid_t ));
    printk("unsigned int  sessionid %ld\n", sizeof(unsigned int ));
    printk("struct seccomp  seccomp %ld\n", sizeof(struct seccomp ));
    printk("u64  parent_exec_id %ld\n", sizeof(u64 ));
    printk("u64  self_exec_id %ld\n", sizeof(u64 ));
    printk("spinlock_t  alloc_lock %ld\n", sizeof(spinlock_t ));
    printk("raw_spinlock_t  pi_lock %ld\n", sizeof(raw_spinlock_t ));
    printk("struct wake_q_node  wake_q %ld\n", sizeof(struct wake_q_node ));
    printk("struct rb_root  pi_waiters %ld\n", sizeof(struct rb_root ));
    printk("struct rb_node * pi_waiters_leftmost %ld\n", sizeof(struct rb_node *));
    printk("struct rt_mutex_waiter * pi_blocked_on %ld\n", sizeof(struct rt_mutex_waiter *));
    printk("void * journal_info %ld\n", sizeof(void *));
    printk("struct bio_list * bio_list %ld\n", sizeof(struct bio_list *));
    printk("struct blk_plug * plug %ld\n", sizeof(struct blk_plug *));
    printk("struct reclaim_state * reclaim_state %ld\n", sizeof(struct reclaim_state *));
    printk("struct backing_dev_info * backing_dev_info %ld\n", sizeof(struct backing_dev_info *));
    printk("struct io_context * io_context %ld\n", sizeof(struct io_context *));
    printk("unsigned long  ptrace_message %ld\n", sizeof(unsigned long ));
    printk("siginfo_t * last_siginfo %ld\n", sizeof(siginfo_t *)); 
    printk("struct task_io_accounting  ioac %ld\n", sizeof(struct task_io_accounting ));
    printk("u64  acct_rss_mem1 %ld\n", sizeof(u64 ));	
    printk("u64  acct_vm_mem1 %ld\n", sizeof(u64 ));	
    printk("cputime_t  acct_timexpd %ld\n", sizeof(cputime_t ));	
    printk("nodemask_t  mems_allowed %ld\n", sizeof(nodemask_t ));	
    printk("seqcount_t  mems_allowed_seq %ld\n", sizeof(seqcount_t ));	
    printk("int  cpuset_mem_spread_rotor %ld\n", sizeof(int ));
    printk("int  cpuset_slab_spread_rotor %ld\n", sizeof(int ));
    printk("struct css_set __rcu * cgroups %ld\n", sizeof(struct css_set __rcu *));
    printk("struct list_head  cg_list %ld\n", sizeof(struct list_head ));
    printk("struct robust_list_head __user * robust_list %ld\n", sizeof(struct robust_list_head __user *));
    printk("struct compat_robust_list_head __user * compat_robust_list %ld\n", sizeof(struct compat_robust_list_head __user *));
    printk("struct list_head  pi_state_list %ld\n", sizeof(struct list_head ));
    printk("struct futex_pi_state * pi_state_cache %ld\n", sizeof(struct futex_pi_state *));
    printk("struct mutex  futex_exit_mutex %ld\n", sizeof(struct mutex ));
    printk("unsigned int  futex_state %ld\n", sizeof(unsigned int ));
    printk("struct perf_event_context * perf_event_ctxp1 %ld\n", sizeof(struct perf_event_context *));
    printk("struct perf_event_context * perf_event_ctxp2 %ld\n", sizeof(struct perf_event_context *));
    printk("struct mutex  perf_event_mutex %ld\n", sizeof(struct mutex ));
    printk("struct list_head  perf_event_list %ld\n", sizeof(struct list_head ));
    printk("struct rcu_head  rcu %ld\n", sizeof(struct rcu_head ));
    printk("struct pipe_inode_info * splice_pipe %ld\n", sizeof(struct pipe_inode_info *));
    printk("struct page_frag  task_frag %ld\n", sizeof(struct page_frag ));
    printk("struct task_delay_info * delays %ld\n", sizeof(struct task_delay_info *));
    printk("int  nr_dirtied %ld\n", sizeof(int ));
    printk("int  nr_dirtied_pause %ld\n", sizeof(int ));
    printk("unsigned long  dirty_paused_when %ld\n", sizeof(unsigned long )); 
    printk("u64  timer_slack_ns %ld\n", sizeof(u64 ));
    printk("u64  default_timer_slack_ns %ld\n", sizeof(u64 ));
    printk("int  curr_ret_stack %ld\n", sizeof(int ));
    printk("struct  ftrace_ret_stack	*ret_stack %ld\n", sizeof(struct ftrace_ret_stack*));
    printk("unsigned long long  ftrace_timestamp %ld\n", sizeof(unsigned long long ));
    printk("atomic_t  trace_overrun %ld\n", sizeof(atomic_t ));
    printk("atomic_t  tracing_graph_pause %ld\n", sizeof(atomic_t ));
    printk("unsigned long  trace %ld\n", sizeof(unsigned long ));
    printk("unsigned long  trace_recursion %ld\n", sizeof(unsigned long ));
    printk("struct mem_cgroup * memcg_in_oom %ld\n", sizeof(struct mem_cgroup *));
    printk("gfp_t  memcg_oom_gfp_mask %ld\n", sizeof(gfp_t ));
    printk("int  memcg_oom_order %ld\n", sizeof(int ));
    printk("unsigned int  memcg_nr_pages_over_high %ld\n", sizeof(unsigned int ));
    printk("unsigned  int	sequential_io %ld\n", sizeof(unsigned ));
    printk("unsigned  int	sequential_io_avg %ld\n", sizeof(unsigned ));
    printk("int  pagefault_disabled %ld\n", sizeof(int ));
    printk("struct task_struct * oom_reaper_list %ld\n", sizeof(struct task_struct *));
    printk("struct thread_struct  thread %ld\n", sizeof(struct thread_struct ));


    printk("prio should be %d\n", MAX_PRIO-20);
    printk("CPU_MASK_ALL is %X\n", CPU_MASK_ALL);

    void* child_task_addr = (void*)0xFFFF8000060bb2e0;
    printk("you give: %p\n", child_task_addr);
    printk("i give: %p\n", virt_to_phys(child_task_addr));

    printk("BIT(VA_BITS - 1) is %X\n", BIT(VA_BITS - 1));
    printk("~PAGE_OFFSET is %X\n", ~PAGE_OFFSET);
    printk("PHYS_OFFSET is %X\n", PHYS_OFFSET);
    printk("kimage_voffset is %X\n", kimage_voffset);

    printk("struct cred %ld\n", sizeof(struct cred));
    printk("atomic_t %ld\n", sizeof(atomic_t));
    printk("void * %ld\n", sizeof(void *));
    printk("unsigned %ld\n", sizeof(unsigned));
    printk("kuid_t %ld\n", sizeof(kuid_t));
    printk("kgid_t %ld\n", sizeof(kgid_t));
    printk("kernel_cap_t %ld\n", sizeof(kernel_cap_t));
    printk("unsigned char %ld\n", sizeof(unsigned char));
    printk("struct key * %ld\n", sizeof(struct key *));
#if defined(CONFIG_DEBUG_CREDENTIALS)
    printk("CONFIG_DEBUG_CREDENTIALS is defined\n");
#else
    printk("CONFIG_DEBUG_CREDENTIALS is not defined\n");
#endif
#if defined(CONFIG_KEYS)
    printk("CONFIG_KEYS is defined\n");
#else
    printk("CONFIG_KEYS is not defined\n");
#endif
#if defined(CONFIG_SECURITY)
    printk("CONFIG_SECURITY is defined\n");
#else
    printk("CONFIG_SECURITY is not defined\n");
#endif

    printk("spinlock_t %ld\n", sizeof(spinlock_t));
    printk("struct mm_struct %ld\n", sizeof(struct mm_struct));
    printk("struct rb_root %ld\n", sizeof(struct rb_root));
    printk("spinlock_t %ld\n", sizeof(spinlock_t));
    printk("struct rw_semaphore %ld\n", sizeof(struct rw_semaphore));
#if defined(CONFIG_MMU)
    printk("CONFIG_MMU is defined\n");
#else
    printk("CONFIG_MMU is not defined\n");
#endif
#if defined(CONFIG_PGTABLES)
    printk("CONFIG_PGTABLE_LEVELS is %d\n", CONFIG_PGTABLE_LEVELS);
#else
    printk("CONFIG_PGTABLE_LEVELS is not defined\n");
#endif
    printk("struct rb_node %ld\n", sizeof(struct rb_node));
    printk("pgprot_t %ld\n", sizeof(pgprot_t));

    printk("PAGE_SHIFT is %d\n", PAGE_SHIFT);
    printk("PMD_SHIFT is %d\n", PMD_SHIFT);
    printk("PGDIR_SHIFT is %d\n", PGDIR_SHIFT);

#if defined(CONFIG_AIO)
    printk("CONFIG_AIO is defined\n");
#else
    printk("CONFIG_AIO is not defined\n");
#endif
#if defined(CONFIG_MEMCG)
    printk("CONFIG_MEMCG is defined\n");
#else
    printk("CONFIG_MEMCG is not defined\n");
#endif
#if defined(CONFIG_MMU_NOTIFIER)
    printk("CONFIG_MMU_NOTIFIER is defined\n");
#else
    printk("CONFIG_MMU_NOTIFIER is not defined\n");
#endif
#if defined(CONFIG_TRANSPARENT_HUGEPAGE)
    printk("CONFIG_TRANSPARENT_HUGEPAGE is defined\n");
#else
    printk("CONFIG_TRANSPARENT_HUGEPAGE is not defined\n");
#endif
#if defined(CONFIG_CPUMASK_OFFSTACK)
    printk("CONFIG_CPUMASK_OFFSTACK is defined\n");
#else
    printk("CONFIG_CPUMASK_OFFSTACK is not defined\n");
#endif
#if defined(CONFIG_COMPACTION)
    printk("CONFIG_COMPACTION is defined\n");
#else
    printk("CONFIG_COMPACTION is not defined\n");
#endif
#if defined(CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH)
    printk("CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH is defined\n");
#else
    printk("CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH is not defined\n");
#endif
#if defined(CONFIG_X86_INTEL_MPX)
    printk("CONFIG_X86_INTEL_MPX is defined\n");
#else
    printk("CONFIG_X86_INTEL_MPX is not defined\n");
#endif
#if defined(CONFIG_HUGETLB_PAGE)
    printk("CONFIG_HUGETLB_PAGE is defined\n");
#else
    printk("CONFIG_HUGETLB_PAGE is not defined\n");
#endif
#if defined(USE_SPLIT_PMD_PTLOCKS)
    printk("USE_SPLIT_PMD_PTLOCKS is %d\n", USE_SPLIT_PMD_PTLOCKS);
#else
    printk("USE_SPLIT_PMD_PTLOCKS is not defined\n");
#endif



    return 0;
}

