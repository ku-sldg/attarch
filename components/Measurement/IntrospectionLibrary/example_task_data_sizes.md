## linux 6.1 aarch64 armv8-a, simulated with qemu-arm-virt
## These are the fields included in the task_struct for linux 6.1
## I will use this to deduce offsets of interesting data and confirm my grasp on the runtime task_struct. In other words, I need to have evidence that my measurement is really measuring what I will claim it measures.

 | type | name | size in bytes (decimal) |
 | --- | --- | --- |
 | struct | task_struct | 3968 |
 | struct thread_info | thread_info | 24 |
 | unsigned int | __state | 4 |
 | void* | stack | 8 |
 | refcount_t | usage | 4 |
 | unsigned int | flags | 4 |
 | unsigned int | ptrace | 4 |
 | int | on_cpu | 4 |
 | struct __call_single_node | wake_entry | 16 |
 | unsigned int | wakee_flips | 4 |
 | unsigned long | wakee_flip_decay_ts | 8 |
 | struct task_struct* | last_wakee | 8 |
 | int | recent_used_cpu | 4 |
 | int | wake_cpu | 4 |
 | int | on_rq | 4 |
 | int | prio | 4 |
 | int | static_prio | 4 |
 | int | normal_prio | 4 |
 | unsigned int | rt_priority | 4 |
 | struct sched_entity | se | 256 |
 | struct sched_rt_entity | rt | 48 |
 | struct sched_dl_entity | dl | 224 |
 | const struct sched_class* | sched_class | 8 |
 | struct task_group* | sched_task_group | 8 |
 | struct sched_statistics | stats | 256 |
 | struct hlist_head | preempt_notifiers | 8 |
 | unsigned int | btrace_seq | 4 |
 | unsigned int | policy | 4 |
 | int | nr_cpus_allowed | 4 |
 | const cpumask_t* | cpus_ptr | 8 |
 | cpumask_t* | user_cpus_ptr | 8 |
 | cpumask_t | cpus_mask | 32 |
 | void* | migration_pending | 8 |
 | unsigned short | migration_disabled | 2 |
 | unsigned short | migration_flags | 2 |
 | int | trc_reader_nesting | 4 |
 | int | trc_ipi_to_cpu | 4 |
 | union rcu_special | trc_reader_special | 4 |
 | struct list_head | trc_holdout_list | 16 |
 | struct list_head | trc_blkd_node | 16 |
 | int | trc_blkd_cpu | 4 |
 | struct sched_info | sched_info | 32 |
 | struct list_head | tasks | 16 |
 | struct plist_node | pushable_tasks | 40 |
 | struct rb_node | pushable_dl_tasks | 24 |
 | struct mm_struct* | mm | 8 |
 | struct mm_struct* | active_mm | 8 |
 | struct task_rss_stat | rss_stat | 20 |
 | int | exit_state | 4 |
 | int | exit_code | 4 |
 | int | exit_signal | 4 |
 | int | pdeath_signal | 4 |
 | unsigned long | jobctl | 8 |
 | unsigned int | personality | 4 |
 | unsigned | sched_reset_on_fork:1 | 4 |
 | unsigned | sched_contributes_to_load:1 | 4 |
 | unsigned | sched_migrated:1 | 4 |
 | unsigned | :0 | 4 |
 | unsigned | sched_remote_wakeup:1 | 4 |
 | unsigned | in_execve:1 | 4 |
 | unsigned | in_iowait:1 | 4 |
 | unsigned | in_user_fault:1 | 4 |
 | unsigned | no_cgroup_migration:1 | 4 |
 | unsigned | frozen:1 | 4 |
 | unsigned | use_memdelay:1 | 4 |
 | unsigned | in_eventfd:1 | 4 |
 | unsigned | in_thrashing:1 | 4 |
 | unsigned long | atomic_flags | 8 |
 | struct restart_block | restart_block | 56 |
 | pid_t | pid | 4 |
 | pid_t | tgid | 4 |
 | unsigned long | stack_canary | 8 |
 | struct task_struct __rcu* | real_parent | 8 |
 | struct task_struct __rcu* | parent | 8 |
 | struct list_head | children | 16 |
 | struct list_head | sibling | 16 |
 | struct task_struct* | group_leader | 8 |
 | struct list_head | ptraced | 16 |
 | struct list_head | ptrace_entry | 16 |
 | struct pid* | thread_pid | 8 |
 | struct hlist_node | pid_links[PIDTYPE_MAX] | 16 |
 | struct list_head | thread_group | 16 |
 | struct list_head | thread_node | 16 |
 | struct completion* | vfork_done | 8 |
 | int __user* | set_child_tid | 8 |
 | int __user* | clear_child_tid | 8 |
 | void* | worker_private | 8 |
 | u64 | utime | 8 |
 | u64 | stime | 8 |
 | u64 | gtime | 8 |
 | struct prev_cputime | prev_cputime | 24 |
 | unsigned long | nvcsw | 8 |
 | unsigned long | nivcsw | 8 |
 | u64 | start_time | 8 |
 | u64 | start_boottime | 8 |
 | unsigned long | min_flt | 8 |
 | unsigned long | maj_flt | 8 |
 | struct posix_cputimers | posix_cputimers | 80 |
 | struct posix_cputimers_work | posix_cputimers_work | 56 |
 | const struct cred* __rcu | ptracer_cred | 8 |
 | const struct cred* __rcu | real_cred | 8 |
 | const struct cred* __rcu | cred | 8 |
 | struct key* | cached_requested_key | 8 |
 | char comm[TASK_COMM_LEN] 16
 | struct nameidata* | nameidata | 8 |
 | struct sysv_sem | sysvsem | 8 |
 | struct sysv_shm | sysvshm | 16 |
 | unsigned long | last_switch_count | 8 |
 | unsigned long | last_switch_time | 8 |
 | struct fs_struct* | fs | 8 |
 | struct files_struct* | files | 8 |
 | struct io_uring_task* | io_uring | 8 |
 | struct nsproxy* | nsproxy | 8 |
 | struct signal_struct* | signal | 8 |
 | struct sighand_struct __rcu* | sighand | 8 |
 | sigset_t | blocked | 8 |
 | sigset_t | real_blocked | 8 |
 | sigset_t | saved_sigmask | 8 |
 | struct sigpending | pending | 24 |
 | unsigned long | sas_ss_sp | 8 |
 | size_t | sas_ss_size | 8 |
 | unsigned int | sas_ss_flags | 4 |
 | struct callback_head* | task_works | 8 |
 | struct audit_context* | audit_context | 8 |
 | kuid_t | loginuid | 4 |
 | unsigned int | sessionid | 4 |
 | struct seccomp | seccomp | 16 |
 | struct syscall_user_dispatch | syscall_dispatch | 0 |
 | u64 | parent_exec_id | 8 |
 | u64 | self_exec_id | 8 |
 | spinlock_t | alloc_lock | 4 |
 | raw_spinlock_t | pi_lock | 4 |
 | struct wake_q_node | wake_q | 8 |
 | struct rb_root_cached | pi_waiters | 16 |
 | struct task_struct* | pi_top_task | 8 |
 | struct rt_mutex_waiter* | pi_blocked_on | 8 |
 | void* | journal_info | 8 |
 | struct bio_list* | bio_list | 8 |
 | struct blk_plug* | plug | 8 |
 | struct reclaim_state* | reclaim_state | 8 |
 | struct backing_dev_info* | backing_dev_info | 8 |
 | struct io_context* | io_context | 8 |
 | struct capture_control* | capture_control | 8 |
 | unsigned long | ptrace_message | 8 |
 | kernel_siginfo_t* | last_siginfo | 8 |
 | struct task_io_accounting | ioac | 56 |
 | u64 | acct_rss_mem1 | 8 |
 | u64 | acct_vm_mem1 | 8 |
 | u64 | acct_timexpd | 8 |
 | nodemask_t | mems_allowed | 8 |
 | seqcount_spinlock_t | mems_allowed_seq | 4 |
 | int | cpuset_mem_spread_rotor | 4 |
 | int | cpuset_slab_spread_rotor | 4 |
 | struct css_set __rcu* | cgroups | 8 |
 | struct list_head | cg_list | 16 |
 | struct robust_list_head __user* | robust_list | 8 |
 | struct compat_robust_list_head __user* | compat_robust_list | 8 |
 | struct list_head | pi_state_list | 16 |
 | struct futex_pi_state* | pi_state_cache | 8 |
 | struct mutex | futex_exit_mutex | 32 |
 | unsigned int | futex_state | 4 |
 | struct perf_event_context * | perf_event_ctxp[perf_nr_task_contexts] | 8
 | struct mutex | perf_event_mutex | 32 |
 | struct list_head | perf_event_list | 16 |
 | struct rseq __user* | rseq | 8 |
 | u32 | rseq_sig | 4 |
 | unsigned long | rseq_event_mask | 8 |
 | struct tlbflush_unmap_batch | tlb_ubc | 0 |
 | refcount_t | rcu_users | 4 |
 | rcu_head | rcu | 16 |
 | refcount_t | rcu_users | 4 |
 | struct rcu_head | rcu | 16 |
 | struct pipe_inode_info* | splice_pipe | 8 |
 | struct page_frag | task_frag | 16 |
 | struct task_delay_info* | delays | 8 |
 | int | nr_dirtied | 4 |
 | int | nr_dirtied_pause | 4 |
 | unsigned long | dirty_paused_when | 8 |
 | u64 | timer_slack_ns | 8 |
 | u64 | default_timer_slack_ns | 8 |
 | int | curr_ret_stack | 4 |
 | int | curr_ret_depth | 4 |
 | struct ftrace_ret_stack* | ret_stack | 8 |
 | unsigned long long | ftrace_timestamp | 8 |
 | atomic_t | trace_overrun | 4 |
 | atomic_t | tracing_graph_pause | 4 |
 | unsigned long | trace_recursion | 8 |
 | struct mem_cgroup* | memcg_in_oom | 8 |
 | gfp_t | memcg_oom_gfp_mask | 4 |
 | int | memcg_oom_order | 4 |
 | unsigned int | memcg_nr_pages_over_high | 4 |
 | struct mem_cgroup* | active_memcg | 8 |
 | struct request_queue* | throttle_queue | 8 |
 | struct uprobe_task* | utask | 8 |
 | unsigned int | sequential_io | 4 |
 | unsigned int | sequential_io_avg | 4 |
 | struct kmap_ctrl | kmap_ctrl | 0 |
 | int | pagefault_disabled | 4 |
 | struct task_struct* | oom_reaper_list | 8 |
 | struct timer_list | oom_reaper_timer | 40 |
 | struct vm_struct* | stack_vm_area | 8 |
 | refcount_t | stack_refcount | 4 |
 | void* | security | 8 |
 | struct bpf_local_storage __rcu* | bpf_storage | 8 |
 | struct bpf_run_ctx* | bpf_ctx | 8 |
 | struct llist_head | kretprobe_instances | 8 |
 | struct thread_struct | thread | 1120 |
