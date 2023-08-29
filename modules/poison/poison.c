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
    printk("page offset is %llx \n", (uint64_t)PAGE_OFFSET);
    /* printk("page END is %llx \n", (uint64_t)PAGE_END); */
    printk("kimage_vaddr is %llx \n", (uint64_t)KIMAGE_VADDR);
    printk("VA_BITS is %llx \n", (uint64_t)VA_BITS);

     printk("\n\n");
     printk("\noffset of group_head is %lu\n", offsetof(struct sched_entity, group_node));
     printk("\n\n");
     printk("size of mm_struct is %lu\n", sizeof(struct mm_struct));
     printk("size of maple_tree is %lu\n", sizeof(struct maple_tree));
     printk("size of maple_node is %lu\n", sizeof(struct maple_node));
     printk("size of enum maple_type is %lu\n", sizeof(enum maple_type));
     printk("size of unsigned long is %lu\n", sizeof(unsigned long));

     printk("\n\n");
     printk("offset of mm in task_struct is %lu\n", offsetof(struct task_struct, mm));
     printk("offset of mm_mt in mm_struct is %lu\n", offsetof(struct mm_struct, mm_mt));
     printk("offset of ma_root in maple_tree is %lu\n", offsetof(struct maple_tree, ma_root));
     printk("offset of pgd in mm_struct is %lu\n", offsetof(struct mm_struct, pgd));

     printk("\n\n");
     printk("offset of parent in maple_node is %lu\n", offsetof(struct maple_node, parent));
     printk("offset of slot in maple_node is %lu\n", offsetof(struct maple_node, slot));
     printk("offset of pad in maple_node is %lu\n", offsetof(struct maple_node, pad));
     printk("offset of rcu in maple_node is %lu\n", offsetof(struct maple_node, rcu));
     printk("offset of piv_parent in maple_node is %lu\n", offsetof(struct maple_node, piv_parent));
     printk("offset of parent_slot in maple_node is %lu\n", offsetof(struct maple_node, parent_slot));
     printk("offset of slot_len in maple_node is %lu\n", offsetof(struct maple_node, slot_len));
     printk("offset of ma_flags in maple_node is %lu\n", offsetof(struct maple_node, ma_flags));
     printk("offset of mr64 in maple_node is %lu\n", offsetof(struct maple_node, mr64));
     printk("offset of ma64 in maple_node is %lu\n", offsetof(struct maple_node, ma64));
     printk("offset of alloc in maple_node is %lu\n", offsetof(struct maple_node, alloc));
     printk("\n\n");

     printk("offset of maple_type in maple_node is %lu\n", offsetof(struct maple_node, type));
     printk("offset of meta in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, meta));
     printk("size of struct maple_metadata is %lu\n", sizeof(struct maple_metadata));
     printk("offset of total in maple_alloc is %lu\n", offsetof(struct maple_alloc, total));
     printk("offset of node_count in maple_alloc is %lu\n", offsetof(struct maple_alloc, node_count));
     printk("offset of request_count in maple_alloc is %lu\n", offsetof(struct maple_alloc, request_count));
     printk("\n\n");
     printk("offset of parent in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, parent));
     printk("offset of pivot in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, pivot));
     printk("offset of slot in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, slot));
     printk("offset of gap in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, gap));
     printk("offset of meta in maple_arange_64 is %lu\n", offsetof(struct maple_arange_64, meta));
     printk("\n\n");
     printk("offset of parent in maple_range_64 is %lu\n", offsetof(struct maple_range_64, parent));
     printk("offset of pivot in maple_range_64 is %lu\n", offsetof(struct maple_range_64, pivot));
     printk("offset of slot in maple_range_64 is %lu\n", offsetof(struct maple_range_64, slot));
     printk("offset of pad in maple_range_64 is %lu\n", offsetof(struct maple_range_64, pad));
     printk("offset of meta in maple_range_64 is %lu\n", offsetof(struct maple_range_64, meta));
     printk("\n\n");
     printk("size of struct maple_arange_64 is %lu\n", sizeof(struct maple_arange_64));
     printk("size of struct maple_range_64 is %lu\n", sizeof(struct maple_range_64));
     printk("size of struct maple_alloc is %lu\n", sizeof(struct maple_alloc));
     printk("\n\n");
     printk("offset of vm_start in vm_area_struct is %lu\n", offsetof(struct vm_area_struct, vm_start));
     printk("offset of vm_end in vm_area_struct is %lu\n", offsetof(struct vm_area_struct, vm_end));
     printk("\n\n");

     printk("offset of thread_info is %lu\n", offsetof(struct task_struct, thread_info));
     printk("offset of __state is %lu\n", offsetof(struct task_struct, __state));
     printk("offset of stack is %lu\n", offsetof(struct task_struct, stack));
     printk("offset of usage is %lu\n", offsetof(struct task_struct, usage));
     printk("offset of flags is %lu\n", offsetof(struct task_struct, flags));
     printk("offset of ptrace is %lu\n", offsetof(struct task_struct, ptrace));
     printk("offset of on_cpu is %lu\n", offsetof(struct task_struct, on_cpu));
     printk("offset of wake_entry is %lu\n", offsetof(struct task_struct, wake_entry));
     printk("offset of wakee_flips is %lu\n", offsetof(struct task_struct, wakee_flips));
     printk("offset of wakee_flip_decay_ts is %lu\n", offsetof(struct task_struct, wakee_flip_decay_ts));
     printk("offset of last_wakee is %lu\n", offsetof(struct task_struct, last_wakee));
     printk("offset of recent_used_cpu is %lu\n", offsetof(struct task_struct, recent_used_cpu));
     printk("offset of wake_cpu is %lu\n", offsetof(struct task_struct, wake_cpu));
     printk("offset of on_rq is %lu\n", offsetof(struct task_struct, on_rq));
     printk("offset of prio is %lu\n", offsetof(struct task_struct, prio));
     printk("offset of static_prio is %lu\n", offsetof(struct task_struct, static_prio));
     printk("offset of normal_prio is %lu\n", offsetof(struct task_struct, normal_prio));
     printk("offset of rt_priority is %lu\n", offsetof(struct task_struct, rt_priority));
     printk("offset of se is %lu\n", offsetof(struct task_struct, se));
     printk("offset of rt is %lu\n", offsetof(struct task_struct, rt));
     printk("offset of dl is %lu\n", offsetof(struct task_struct, dl));
     printk("offset of sched_class is %lu\n", offsetof(struct task_struct, sched_class));
     printk("offset of sched_task_group is %lu\n", offsetof(struct task_struct, sched_task_group));
     printk("offset of stats is %lu\n", offsetof(struct task_struct, stats));
     printk("offset of preempt_notifiers is %lu\n", offsetof(struct task_struct, preempt_notifiers));
     printk("offset of btrace_seq is %lu\n", offsetof(struct task_struct, btrace_seq));
     printk("offset of policy is %lu\n", offsetof(struct task_struct, policy));
     printk("offset of nr_cpus_allowed is %lu\n", offsetof(struct task_struct, nr_cpus_allowed));
     printk("offset of cpus_ptr is %lu\n", offsetof(struct task_struct, cpus_ptr));
     printk("offset of user_cpus_ptr is %lu\n", offsetof(struct task_struct, user_cpus_ptr));
     printk("offset of cpus_mask is %lu\n", offsetof(struct task_struct, cpus_mask));
     printk("offset of migration_pending is %lu\n", offsetof(struct task_struct, migration_pending));
     printk("offset of migration_disabled is %lu\n", offsetof(struct task_struct, migration_disabled));
     printk("offset of migration_flags is %lu\n", offsetof(struct task_struct, migration_flags));
     printk("offset of trc_reader_nesting is %lu\n", offsetof(struct task_struct, trc_reader_nesting));
     printk("offset of trc_ipi_to_cpu is %lu\n", offsetof(struct task_struct, trc_ipi_to_cpu));
     printk("offset of trc_reader_special is %lu\n", offsetof(struct task_struct, trc_reader_special));
     printk("offset of trc_holdout_list is %lu\n", offsetof(struct task_struct, trc_holdout_list));
     printk("offset of trc_blkd_node is %lu\n", offsetof(struct task_struct, trc_blkd_node));
     printk("offset of trc_blkd_cpu is %lu\n", offsetof(struct task_struct, trc_blkd_cpu));
     printk("offset of sched_info is %lu\n", offsetof(struct task_struct, sched_info));
     printk("offset of tasks is %lu\n", offsetof(struct task_struct, tasks));
     printk("offset of pushable_tasks is %lu\n", offsetof(struct task_struct, pushable_tasks));
     printk("offset of pushable_dl_tasks is %lu\n", offsetof(struct task_struct, pushable_dl_tasks));
     printk("offset of mm is %lu\n", offsetof(struct task_struct, mm));
     printk("offset of active_mm is %lu\n", offsetof(struct task_struct, active_mm));
     printk("offset of rss_stat is %lu\n", offsetof(struct task_struct, rss_stat));
     printk("offset of exit_state is %lu\n", offsetof(struct task_struct, exit_state));
     printk("offset of exit_code is %lu\n", offsetof(struct task_struct, exit_code));
     printk("offset of exit_signal is %lu\n", offsetof(struct task_struct, exit_signal));
     printk("offset of pdeath_signal is %lu\n", offsetof(struct task_struct, pdeath_signal));
     printk("offset of jobctl is %lu\n", offsetof(struct task_struct, jobctl));
     printk("offset of personality is %lu\n", offsetof(struct task_struct, personality));
     /* printk("offset of sched_reset_on_fork is %lu\n", offsetof(struct task_struct, sched_reset_on_fork)); */
     /* printk("offset of sched_contributes_to_load is %lu\n", offsetof(struct task_struct, sched_contributes_to_load)); */
     /* printk("offset of sched_migrated is %lu\n", offsetof(struct task_struct, sched_migrated)); */
   // printk("size of unsigned :0 is %lu\n", sizeof(unsigned));
     /* printk("offset of sched_remote_wakeup is %lu\n", offsetof(struct task_struct, sched_remote_wakeup)); */
     /* printk("offset of in_execve is %lu\n", offsetof(struct task_struct, in_execve)); */
     /* printk("offset of in_iowait is %lu\n", offsetof(struct task_struct, in_iowait)); */
     /* printk("offset of in_user_fault is %lu\n", offsetof(struct task_struct, in_user_fault)); */
     /* printk("offset of no_cgroup_migration is %lu\n", offsetof(struct task_struct, no_cgroup_migration)); */
     /* printk("offset of frozen is %lu\n", offsetof(struct task_struct, frozen)); */
     /* printk("offset of use_memdelay is %lu\n", offsetof(struct task_struct, use_memdelay)); */
     /* printk("offset of in_eventfd is %lu\n", offsetof(struct task_struct, in_eventfd)); */
     /* printk("offset of in_thrashing is %lu\n", offsetof(struct task_struct, in_thrashing)); */
     printk("offset of atomic_flags is %lu\n", offsetof(struct task_struct, atomic_flags));
     printk("offset of restart_block is %lu\n", offsetof(struct task_struct, restart_block));
     printk("offset of pid is %lu\n", offsetof(struct task_struct, pid));
     printk("offset of tgid is %lu\n", offsetof(struct task_struct, tgid));
     printk("offset of stack_canary is %lu\n", offsetof(struct task_struct, stack_canary));
     printk("offset of real_parent is %lu\n", offsetof(struct task_struct, real_parent));
     printk("offset of parent is %lu\n", offsetof(struct task_struct, parent));
     printk("offset of children is %lu\n", offsetof(struct task_struct, children));
     printk("offset of sibling is %lu\n", offsetof(struct task_struct, sibling));
     printk("offset of group_leader is %lu\n", offsetof(struct task_struct, group_leader));
     printk("offset of ptraced is %lu\n", offsetof(struct task_struct, ptraced));
     printk("offset of ptrace_entry is %lu\n", offsetof(struct task_struct, ptrace_entry));
     printk("offset of thread_pid is %lu\n", offsetof(struct task_struct, thread_pid));
     printk("offset of pid_links is %lu\n", offsetof(struct task_struct, pid_links));
     printk("offset of thread_group is %lu\n", offsetof(struct task_struct, thread_group));
     printk("offset of thread_node is %lu\n", offsetof(struct task_struct, thread_node));
     printk("offset of vfork_done is %lu\n", offsetof(struct task_struct, vfork_done));
     printk("offset of set_child_tid is %lu\n", offsetof(struct task_struct, set_child_tid));
     printk("offset of clear_child_tid is %lu\n", offsetof(struct task_struct, clear_child_tid));
     printk("offset of worker_private is %lu\n", offsetof(struct task_struct, worker_private));
     printk("offset of utime is %lu\n", offsetof(struct task_struct, utime));
     printk("offset of stime is %lu\n", offsetof(struct task_struct, stime));
     printk("offset of gtime is %lu\n", offsetof(struct task_struct, gtime));
     printk("offset of prev_cputime is %lu\n", offsetof(struct task_struct, prev_cputime));
     printk("offset of nvcsw is %lu\n", offsetof(struct task_struct, nvcsw));
     printk("offset of nivcsw is %lu\n", offsetof(struct task_struct, nivcsw));
     printk("offset of start_time is %lu\n", offsetof(struct task_struct, start_time));
     printk("offset of start_boottime is %lu\n", offsetof(struct task_struct, start_boottime));
     printk("offset of min_flt is %lu\n", offsetof(struct task_struct, min_flt));
     printk("offset of maj_flt is %lu\n", offsetof(struct task_struct, maj_flt));
     printk("offset of posix_cputimers is %lu\n", offsetof(struct task_struct, posix_cputimers));
     printk("offset of posix_cputimers_work is %lu\n", offsetof(struct task_struct, posix_cputimers_work));
     printk("offset of ptracer_cred is %lu\n", offsetof(struct task_struct, ptracer_cred));
     printk("offset of real_cred is %lu\n", offsetof(struct task_struct, real_cred));
     printk("offset of cred is %lu\n", offsetof(struct task_struct, cred));
     printk("offset of cached_requested_key is %lu\n", offsetof(struct task_struct, cached_requested_key));
     printk("offset of comm is %lu\n", offsetof(struct task_struct, comm));
     printk("offset of nameidata is %lu\n", offsetof(struct task_struct, nameidata));
     printk("offset of sysvsem is %lu\n", offsetof(struct task_struct, sysvsem));
     printk("offset of sysvshm is %lu\n", offsetof(struct task_struct, sysvshm));
     printk("offset of last_switch_count is %lu\n", offsetof(struct task_struct, last_switch_count));
     printk("offset of last_switch_time is %lu\n", offsetof(struct task_struct, last_switch_time));
     printk("offset of fs is %lu\n", offsetof(struct task_struct, fs));
     printk("offset of files is %lu\n", offsetof(struct task_struct, files));
     printk("offset of io_uring is %lu\n", offsetof(struct task_struct, io_uring));
     printk("offset of nsproxy is %lu\n", offsetof(struct task_struct, nsproxy));
     printk("offset of signal is %lu\n", offsetof(struct task_struct, signal));
     printk("offset of sighand is %lu\n", offsetof(struct task_struct, sighand));
     printk("offset of blocked is %lu\n", offsetof(struct task_struct, blocked));
     printk("offset of real_blocked is %lu\n", offsetof(struct task_struct, real_blocked));
     printk("offset of saved_sigmask is %lu\n", offsetof(struct task_struct, saved_sigmask));
     printk("offset of pending is %lu\n", offsetof(struct task_struct, pending));
     printk("offset of sas_ss_sp is %lu\n", offsetof(struct task_struct, sas_ss_sp));
     printk("offset of sas_ss_size is %lu\n", offsetof(struct task_struct, sas_ss_size));
     printk("offset of sas_ss_flags is %lu\n", offsetof(struct task_struct, sas_ss_flags));
     printk("offset of task_works is %lu\n", offsetof(struct task_struct, task_works));
     printk("offset of audit_context is %lu\n", offsetof(struct task_struct, audit_context));
     printk("offset of loginuid is %lu\n", offsetof(struct task_struct, loginuid));
     printk("offset of sessionid is %lu\n", offsetof(struct task_struct, sessionid));
     printk("offset of seccomp is %lu\n", offsetof(struct task_struct, seccomp));
     printk("offset of syscall_dispatch is %lu\n", offsetof(struct task_struct, syscall_dispatch));
     printk("offset of parent_exec_id is %lu\n", offsetof(struct task_struct, parent_exec_id));
     printk("offset of self_exec_id is %lu\n", offsetof(struct task_struct, self_exec_id));
     printk("offset of alloc_lock is %lu\n", offsetof(struct task_struct, alloc_lock));
     printk("offset of pi_lock is %lu\n", offsetof(struct task_struct, pi_lock));
     printk("offset of wake_q is %lu\n", offsetof(struct task_struct, wake_q));
     printk("offset of pi_waiters is %lu\n", offsetof(struct task_struct, pi_waiters));
     printk("offset of pi_top_task is %lu\n", offsetof(struct task_struct, pi_top_task));
     printk("offset of pi_blocked_on is %lu\n", offsetof(struct task_struct, pi_blocked_on));
     printk("offset of journal_info is %lu\n", offsetof(struct task_struct, journal_info));
     printk("offset of bio_list is %lu\n", offsetof(struct task_struct, bio_list));
     printk("offset of plug is %lu\n", offsetof(struct task_struct, plug));
     printk("offset of reclaim_state is %lu\n", offsetof(struct task_struct, reclaim_state));
     printk("offset of backing_dev_info is %lu\n", offsetof(struct task_struct, backing_dev_info));
     printk("offset of io_context is %lu\n", offsetof(struct task_struct, io_context));
     printk("offset of capture_control is %lu\n", offsetof(struct task_struct, capture_control));
     printk("offset of ptrace_message is %lu\n", offsetof(struct task_struct, ptrace_message));
     printk("offset of last_siginfo is %lu\n", offsetof(struct task_struct, last_siginfo));
     printk("offset of ioac is %lu\n", offsetof(struct task_struct, ioac));
     printk("offset of acct_rss_mem1 is %lu\n", offsetof(struct task_struct, acct_rss_mem1));
     printk("offset of acct_vm_mem1 iis %lu\n", offsetof(struct task_struct, acct_vm_mem1));
     printk("offset of acct_timexpd is %lu\n", offsetof(struct task_struct, acct_timexpd));
     printk("offset of mems_allowed is %lu\n", offsetof(struct task_struct, mems_allowed));
     printk("offset of mems_allowed_seq is %lu\n", offsetof(struct task_struct, mems_allowed_seq));
     printk("offset of cpuset_mem_spread_rotor is %lu\n", offsetof(struct task_struct, cpuset_mem_spread_rotor));
     printk("offset of cpuset_slab_spread_rotor is %lu\n", offsetof(struct task_struct, cpuset_slab_spread_rotor));
     printk("offset of cgroups is %lu\n", offsetof(struct task_struct, cgroups));
     printk("offset of cg_list is %lu\n", offsetof(struct task_struct, cg_list));
     printk("offset of robust_list is %lu\n", offsetof(struct task_struct, robust_list));
     printk("offset of compat_robust_list is %lu\n", offsetof(struct task_struct, compat_robust_list));
     printk("offset of pi_state_list is %lu\n", offsetof(struct task_struct, pi_state_list));
     printk("offset of pi_state_cache is %lu\n", offsetof(struct task_struct, pi_state_cache));
     printk("offset of futex_exit_mutex is %lu\n", offsetof(struct task_struct, futex_exit_mutex));
     printk("offset of futex_state is %lu\n", offsetof(struct task_struct, futex_state));
     printk("offset of perf_event_ctxp is %lu\n", offsetof(struct task_struct, perf_event_ctxp));
     printk("offset of perf_event_mutex is %lu\n", offsetof(struct task_struct, perf_event_mutex));
     printk("offset of perf_event_list is %lu\n", offsetof(struct task_struct, perf_event_list));
     printk("offset of rseq is %lu\n", offsetof(struct task_struct, rseq));
     printk("offset of rseq_sig is %lu\n", offsetof(struct task_struct, rseq_sig));
     printk("offset of rseq_event_mask is %lu\n", offsetof(struct task_struct, rseq_event_mask));
     printk("offset of tlb_ubc is %lu\n", offsetof(struct task_struct, tlb_ubc));
    /* printk("size of union is\n"); */
     printk("offset of rcu_users is %lu\n", offsetof(struct task_struct, rcu_users));
     printk("offset of rcu is %lu\n", offsetof(struct task_struct, rcu));
     printk("offset of rcu_users is %lu\n", offsetof(struct task_struct, rcu_users));
     printk("offset of rcu is %lu\n", offsetof(struct task_struct, rcu));
     printk("offset of splice_pipe is %lu\n", offsetof(struct task_struct, splice_pipe));
     printk("offset of task_frag is %lu\n", offsetof(struct task_struct, task_frag));
     printk("offset of delays is %lu\n", offsetof(struct task_struct, delays));
     printk("offset of nr_dirtied is %lu\n", offsetof(struct task_struct, nr_dirtied));
     printk("offset of nr_dirtied_pause is %lu\n", offsetof(struct task_struct, nr_dirtied_pause));
     printk("offset of dirty_paused_when is %lu\n", offsetof(struct task_struct, dirty_paused_when));
     printk("offset of timer_slack_ns is %lu\n", offsetof(struct task_struct, timer_slack_ns));
     printk("offset of default_timer_slack_ns is %lu\n", offsetof(struct task_struct, default_timer_slack_ns));
     printk("offset of curr_ret_stack is %lu\n", offsetof(struct task_struct, curr_ret_stack));
     printk("offset of curr_ret_depth is %lu\n", offsetof(struct task_struct, curr_ret_depth));
     printk("offset of ret_stack is %lu\n", offsetof(struct task_struct, ret_stack));
     printk("offset of ftrace_timestamp is %lu\n", offsetof(struct task_struct, ftrace_timestamp));
     printk("offset of trace_overrun is %lu\n", offsetof(struct task_struct, trace_overrun));
     printk("offset of tracing_graph_pause is %lu\n", offsetof(struct task_struct, tracing_graph_pause));
     printk("offset of trace_recursion is %lu\n", offsetof(struct task_struct, trace_recursion));
     printk("offset of memcg_in_oom is %lu\n", offsetof(struct task_struct, memcg_in_oom));
     printk("offset of memcg_oom_gfp_mask is %lu\n", offsetof(struct task_struct, memcg_oom_gfp_mask));
     printk("offset of memcg_oom_order is %lu\n", offsetof(struct task_struct, memcg_oom_order));
     printk("offset of memcg_nr_pages_over_high is %lu\n", offsetof(struct task_struct, memcg_nr_pages_over_high));
     printk("offset of active_memcg is %lu\n", offsetof(struct task_struct, active_memcg));
     printk("offset of throttle_queue is %lu\n", offsetof(struct task_struct, throttle_queue));
     printk("offset of utask is %lu\n", offsetof(struct task_struct, utask));
     printk("offset of sequential_io is %lu\n", offsetof(struct task_struct, sequential_io));
     printk("offset of sequential_io_avg is %lu\n", offsetof(struct task_struct, sequential_io_avg));
     printk("offset of kmap_ctrl is %lu\n", offsetof(struct task_struct, kmap_ctrl));
     printk("offset of pagefault_disabled is %lu\n", offsetof(struct task_struct, pagefault_disabled));
     printk("offset of oom_reaper_list is %lu\n", offsetof(struct task_struct, oom_reaper_list));
     printk("offset of oom_reaper_timer is %lu\n", offsetof(struct task_struct, oom_reaper_timer));
     printk("offset of stack_vm_area is %lu\n", offsetof(struct task_struct, stack_vm_area));
     printk("offset of stack_refcount is %lu\n", offsetof(struct task_struct, stack_refcount));
     printk("offset of security is %lu\n", offsetof(struct task_struct, security));
     printk("offset of bpf_storage is %lu\n", offsetof(struct task_struct, bpf_storage));
     printk("offset of bpf_ctx is %lu\n", offsetof(struct task_struct, bpf_ctx));
     printk("offset of kretprobe_instances is %lu\n", offsetof(struct task_struct, kretprobe_instances));
     printk("offset of thread is %lu\n", offsetof(struct task_struct, thread));

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

