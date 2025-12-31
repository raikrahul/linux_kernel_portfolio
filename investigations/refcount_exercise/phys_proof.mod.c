#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd272d446, "__fentry__" },
	{ 0xa009a8d7, "find_get_pid" },
	{ 0x774b89a0, "get_pid_task" },
	{ 0x69fc1ea6, "__tracepoint_mmap_lock_start_locking" },
	{ 0xa59da3c0, "down_read" },
	{ 0x69fc1ea6, "__tracepoint_mmap_lock_acquire_returned" },
	{ 0xf296206e, "pgdir_shift" },
	{ 0xb1ad3f2f, "boot_cpu_data" },
	{ 0x69fc1ea6, "__tracepoint_mmap_lock_released" },
	{ 0xa59da3c0, "up_read" },
	{ 0xbaf098df, "put_pid" },
	{ 0x095159b2, "physical_mask" },
	{ 0x1bdf2bc8, "sme_me_mask" },
	{ 0xf296206e, "ptrs_per_p4d" },
	{ 0xb3f8c2e9, "pv_ops" },
	{ 0xd272d446, "BUG_func" },
	{ 0xbd03ed67, "page_offset_base" },
	{ 0x6c91d9ce, "__mmap_lock_do_trace_acquire_returned" },
	{ 0x0501b957, "__mmap_lock_do_trace_start_locking" },
	{ 0x0501b957, "__mmap_lock_do_trace_released" },
	{ 0x7f6d5e1a, "__put_task_struct" },
	{ 0x2520ea93, "refcount_warn_saturate" },
	{ 0x82fd7238, "__ubsan_handle_shift_out_of_bounds" },
	{ 0xc2614bbe, "param_ops_ulong" },
	{ 0xc2614bbe, "param_ops_int" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xa009a8d7,
	0x774b89a0,
	0x69fc1ea6,
	0xa59da3c0,
	0x69fc1ea6,
	0xf296206e,
	0xb1ad3f2f,
	0x69fc1ea6,
	0xa59da3c0,
	0xbaf098df,
	0x095159b2,
	0x1bdf2bc8,
	0xf296206e,
	0xb3f8c2e9,
	0xd272d446,
	0xbd03ed67,
	0x6c91d9ce,
	0x0501b957,
	0x0501b957,
	0x7f6d5e1a,
	0x2520ea93,
	0x82fd7238,
	0xc2614bbe,
	0xc2614bbe,
	0xe8213e80,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"find_get_pid\0"
	"get_pid_task\0"
	"__tracepoint_mmap_lock_start_locking\0"
	"down_read\0"
	"__tracepoint_mmap_lock_acquire_returned\0"
	"pgdir_shift\0"
	"boot_cpu_data\0"
	"__tracepoint_mmap_lock_released\0"
	"up_read\0"
	"put_pid\0"
	"physical_mask\0"
	"sme_me_mask\0"
	"ptrs_per_p4d\0"
	"pv_ops\0"
	"BUG_func\0"
	"page_offset_base\0"
	"__mmap_lock_do_trace_acquire_returned\0"
	"__mmap_lock_do_trace_start_locking\0"
	"__mmap_lock_do_trace_released\0"
	"__put_task_struct\0"
	"refcount_warn_saturate\0"
	"__ubsan_handle_shift_out_of_bounds\0"
	"param_ops_ulong\0"
	"param_ops_int\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2CD8CAFF1B47164C3EAF53A");
