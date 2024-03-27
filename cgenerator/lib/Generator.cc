#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Path.h"

#include "Generator.h"
#include "CallGraph.h"

using namespace llvm;

cl::list<std::string> InputFilenames(
		cl::Positional, cl::OneOrMore, cl::desc("<input bitcode files>"));


GlobalContext GlobalCtx;

void run(CustomCallGraph &CGPass){
	
	//doModulePass and store all the functions and the callee 
	//functions inside the cgMap, fMap
	ModuleList &modules = GlobalCtx.Modules;
	
	errs()<< "Processing "<< modules.size() << " modules.\n";
	ModuleList::iterator i, e;
	
	for(i = modules.begin(), e = modules.end(); i!=e; i++){
		CGPass.doModulePass(i->first);
	}


	errs()<< "ModulePass Successfully Completed\n";

	errs()<< "Printing Stats after ModulePass\n";
	// questions to answer
	// 1. Number of functions in the callgraph
	errs()<< "Number of functions: " << GlobalCtx.fList.size() <<"\n";

	//^^^ let's think about some more stats and fill above
	std::string arr[] = {"bpf_rc_repeat","bpf_rc_keydown","bpf_rc_pointer_rel","bpf_cgrp_storage_get","bpf_cgrp_storage_delete","bpf_inode_storage_get","bpf_inode_storage_delete","bpf_for_each_map_elem","bpf_loop","bpf_bprm_opts_set","bpf_ima_inode_hash","bpf_ima_file_hash","bpf_get_attach_cookie","bpf_task_storage_get_recur","bpf_task_storage_get","bpf_task_storage_delete_recur","bpf_task_storage_delete","bpf_btf_find_by_name_kind","bpf_get_local_storage","bpf_get_retval","bpf_set_retval","bpf_sysctl_get_name","bpf_sysctl_get_current_value","bpf_sysctl_get_new_value","bpf_sysctl_set_new_value","bpf_get_netns_cookie_sockopt","bpf_user_rnd_u32","bpf_get_raw_cpu_id","bpf_map_lookup_elem","bpf_map_update_elem","bpf_map_delete_elem","bpf_map_push_elem","bpf_map_pop_elem","bpf_map_peek_elem","bpf_map_lookup_percpu_elem","bpf_get_smp_processor_id","bpf_get_numa_node_id","bpf_ktime_get_ns","bpf_ktime_get_boot_ns","bpf_ktime_get_coarse_ns","bpf_ktime_get_tai_ns","bpf_get_current_pid_tgid","bpf_get_current_uid_gid","bpf_get_current_comm","bpf_spin_lock","bpf_spin_unlock","bpf_jiffies64","bpf_get_current_cgroup_id","bpf_get_current_ancestor_cgroup_id","bpf_strtol","bpf_strtoul","bpf_strncmp","bpf_get_ns_current_pid_tgid","bpf_event_output_data","bpf_copy_from_user","bpf_copy_from_user_task","bpf_per_cpu_ptr","bpf_this_cpu_ptr","bpf_snprintf","bpf_timer_init","bpf_timer_set_callback","bpf_timer_start","bpf_timer_cancel","bpf_kptr_xchg","bpf_dynptr_from_mem","bpf_dynptr_read","bpf_dynptr_write","bpf_dynptr_data","bpf_ringbuf_reserve","bpf_ringbuf_submit","bpf_ringbuf_discard","bpf_ringbuf_output","bpf_ringbuf_query","bpf_ringbuf_reserve_dynptr","bpf_ringbuf_submit_dynptr","bpf_ringbuf_discard_dynptr","bpf_user_ringbuf_drain","bpf_get_stackid","bpf_get_stackid_pe","bpf_get_stack","bpf_get_task_stack","bpf_get_stack_pe","bpf_sys_bpf","bpf_sys_close","bpf_kallsyms_lookup_name","bpf_find_vma","bpf_override_return","bpf_probe_read_user","bpf_probe_read_user_str","bpf_probe_read_kernel","bpf_probe_read_kernel_str","bpf_probe_read_compat","bpf_probe_read_compat_str","bpf_probe_write_user","bpf_trace_printk","bpf_trace_vprintk","bpf_seq_printf","bpf_seq_write","bpf_seq_printf_btf","bpf_perf_event_read","bpf_perf_event_read_value","bpf_perf_event_output","bpf_get_current_task","bpf_get_current_task_btf","bpf_task_pt_regs","bpf_current_task_under_cgroup","bpf_send_signal","bpf_send_signal_thread","bpf_d_path","bpf_snprintf_btf","bpf_get_func_ip_tracing","bpf_get_func_ip_kprobe","bpf_get_func_ip_kprobe_multi","bpf_get_attach_cookie_kprobe_multi","bpf_get_func_ip_uprobe_multi","bpf_get_attach_cookie_uprobe_multi","bpf_get_attach_cookie_trace","bpf_get_attach_cookie_pe","bpf_get_attach_cookie_tracing","bpf_get_branch_snapshot","get_func_arg","get_func_ret","get_func_arg_cnt","bpf_perf_event_output_tp","bpf_get_stackid_tp","bpf_get_stack_tp","bpf_perf_prog_read_value","bpf_read_branch_records","bpf_perf_event_output_raw_tp","bpf_get_stackid_raw_tp","bpf_get_stack_raw_tp",
		"bpf_test_func", "bpf_sk_storage_get","bpf_sk_storage_delete","bpf_sk_storage_get_tracing","bpf_sk_storage_delete_tracing","bpf_skb_get_pay_offset","bpf_skb_get_nlattr","bpf_skb_get_nlattr_nest","bpf_skb_load_helper_8","bpf_skb_load_helper_8_no_cache","bpf_skb_load_helper_16","bpf_skb_load_helper_16_no_cache","bpf_skb_load_helper_32","bpf_skb_load_helper_32_no_cache","bpf_skb_store_bytes","bpf_skb_load_bytes","bpf_flow_dissector_load_bytes","bpf_skb_load_bytes_relative","bpf_skb_pull_data","bpf_sk_fullsock","sk_skb_pull_data","bpf_l3_csum_replace","bpf_l4_csum_replace","bpf_csum_diff","bpf_csum_update","bpf_csum_level","bpf_clone_redirect","bpf_redirect","bpf_redirect_peer","bpf_redirect_neigh","bpf_msg_apply_bytes","bpf_msg_cork_bytes","bpf_msg_pull_data","bpf_msg_push_data","bpf_msg_pop_data","bpf_get_cgroup_classid_curr","bpf_skb_cgroup_classid","bpf_get_cgroup_classid","bpf_get_route_realm","bpf_get_hash_recalc","bpf_set_hash_invalid","bpf_set_hash","bpf_skb_vlan_push","bpf_skb_vlan_pop","bpf_skb_change_proto","bpf_skb_change_type","sk_skb_adjust_room","bpf_skb_adjust_room","bpf_skb_change_tail","sk_skb_change_tail","bpf_skb_change_head","sk_skb_change_head","bpf_xdp_get_buff_len","bpf_xdp_adjust_head","bpf_xdp_load_bytes","bpf_xdp_store_bytes","bpf_xdp_adjust_tail","bpf_xdp_adjust_meta","bpf_xdp_redirect","bpf_xdp_redirect_map","bpf_skb_event_output","bpf_skb_get_tunnel_key","bpf_skb_get_tunnel_opt","bpf_skb_set_tunnel_key","bpf_skb_set_tunnel_opt","bpf_skb_under_cgroup","bpf_skb_cgroup_id","bpf_skb_ancestor_cgroup_id","bpf_sk_cgroup_id","bpf_sk_ancestor_cgroup_id","bpf_xdp_event_output","bpf_get_socket_cookie","bpf_get_socket_cookie_sock_addr","bpf_get_socket_cookie_sock","bpf_get_socket_ptr_cookie","bpf_get_socket_cookie_sock_ops","bpf_get_netns_cookie_sock","bpf_get_netns_cookie_sock_addr","bpf_get_netns_cookie_sock_ops","bpf_get_netns_cookie_sk_msg","bpf_get_socket_uid","bpf_sk_setsockopt","bpf_sk_getsockopt","bpf_unlocked_sk_setsockopt","bpf_unlocked_sk_getsockopt","bpf_sock_addr_setsockopt","bpf_sock_addr_getsockopt","bpf_sock_ops_setsockopt","bpf_sock_ops_getsockopt","bpf_sock_ops_cb_flags_set","bpf_bind","bpf_skb_get_xfrm_state","bpf_xdp_fib_lookup","bpf_skb_fib_lookup","bpf_skb_check_mtu","bpf_xdp_check_mtu","bpf_lwt_in_push_encap","bpf_lwt_xmit_push_encap","bpf_lwt_seg6_store_bytes","bpf_lwt_seg6_action","bpf_lwt_seg6_adjust_srh","bpf_skc_lookup_tcp","bpf_sk_lookup_tcp","bpf_sk_lookup_udp","bpf_tc_skc_lookup_tcp","bpf_tc_sk_lookup_tcp","bpf_tc_sk_lookup_udp","bpf_sk_release","bpf_xdp_sk_lookup_udp","bpf_xdp_skc_lookup_tcp","bpf_xdp_sk_lookup_tcp","bpf_sock_addr_skc_lookup_tcp","bpf_sock_addr_sk_lookup_tcp","bpf_sock_addr_sk_lookup_udp","bpf_tcp_sock","bpf_get_listener_sock","bpf_skb_ecn_set_ce","bpf_tcp_check_syncookie","bpf_tcp_gen_syncookie","bpf_sk_assign","bpf_sock_ops_load_hdr_opt","bpf_sock_ops_store_hdr_opt","bpf_sock_ops_reserve_hdr_opt","bpf_skb_set_tstamp","bpf_tcp_raw_gen_syncookie_ipv4",
		"bpf_tcp_raw_gen_syncookie_ipv6","bpf_tcp_raw_check_syncookie_ipv4","bpf_tcp_raw_check_syncookie_ipv6","sk_select_reuseport","sk_reuseport_load_bytes","sk_reuseport_load_bytes_relative","bpf_sk_lookup_assign","bpf_skc_to_tcp6_sock","bpf_skc_to_tcp_sock","bpf_skc_to_tcp_timewait_sock","bpf_skc_to_tcp_request_sock","bpf_skc_to_udp6_sock","bpf_skc_to_unix_sock","bpf_skc_to_mptcp_sock","bpf_sock_from_file","bpf_sock_map_update","bpf_sk_redirect_map","bpf_msg_redirect_map","bpf_sock_hash_update","bpf_sk_redirect_hash","bpf_msg_redirect_hash","bpf_tcp_send_ack"};

	int size = sizeof(arr)/sizeof(arr[0]);

	for(int i = 0; i < size; i++) {
		CGPass.callGraphResolver(arr[i]);
	}

	//CGPass.callGraphResolver("bpf_spin_lock");
	//CGPass.callGraphResolver("bpf_get_stackid");
	
	
}

void printResults(){
	errs()<< "printing call graph results\n";
	for(llvm::Function *F : GlobalCtx.fList){
		errs() << "Function: "<<F->getName()<<"\n";	
		for(auto iter : GlobalCtx.ccMap[F]) {
			errs() << "\t" << iter.F->getName() <<"\n";
		}

		for(auto iter : GlobalCtx.indirCCMap[F]) {
			errs() << "\t" << iter <<"\n";
		}
	}
}


int main(int argc, char** argv) {
	
	// Print a stack trace if we signal out.
	sys::PrintStackTraceOnErrorSignal(argv[0]);
	PrettyStackTraceProgram X(argc, argv);

	cl::ParseCommandLineOptions(argc, argv, "global analysis\n");
	SMDiagnostic Err;
	

	// Loading modules
	errs() << "Total " << InputFilenames.size() << " file(s)\n";
	for (unsigned i = 0; i < InputFilenames.size(); ++i) {

		LLVMContext *LLVMCtx = new LLVMContext();
		std::unique_ptr<Module> M = parseIRFile(InputFilenames[i], Err, *LLVMCtx);

		if (M == NULL) {
			errs() << argv[0] << ": error loading file '"
				<< InputFilenames[i] << "'\n";
			continue;
		}

		Module *Module = M.release();
		StringRef MName = StringRef(strdup(InputFilenames[i].data()));
		GlobalCtx.Modules.push_back(std::make_pair(Module, MName));
		GlobalCtx.ModuleMaps[Module] = InputFilenames[i];
	}

	CustomCallGraph CGPass(&GlobalCtx);

	run(CGPass);

//	printResults();

	return 0;
}
