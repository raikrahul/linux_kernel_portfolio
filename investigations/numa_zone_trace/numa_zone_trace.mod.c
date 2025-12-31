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
	{ 0xaed2e23c, "node_states" },
	{ 0x3a645690, "__bitmap_weight" },
	{ 0x494c552b, "_find_first_bit" },
	{ 0x555f872b, "node_data" },
	{ 0x86632fd6, "_find_next_bit" },
	{ 0xb54d37ac, "alloc_pages_noprof" },
	{ 0xbd03ed67, "vmemmap_base" },
	{ 0x44decd6f, "hugetlb_optimize_vmemmap_key" },
	{ 0x44decd6f, "devmap_managed_key" },
	{ 0x3009e428, "__put_devmap_managed_folio_refs" },
	{ 0x23b336c6, "__folio_put" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xaed2e23c,
	0x3a645690,
	0x494c552b,
	0x555f872b,
	0x86632fd6,
	0xb54d37ac,
	0xbd03ed67,
	0x44decd6f,
	0x44decd6f,
	0x3009e428,
	0x23b336c6,
	0x90a48d82,
	0xe8213e80,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"node_states\0"
	"__bitmap_weight\0"
	"_find_first_bit\0"
	"node_data\0"
	"_find_next_bit\0"
	"alloc_pages_noprof\0"
	"vmemmap_base\0"
	"hugetlb_optimize_vmemmap_key\0"
	"devmap_managed_key\0"
	"__put_devmap_managed_folio_refs\0"
	"__folio_put\0"
	"__ubsan_handle_out_of_bounds\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C3CC8B2800B5C6D0C27D5C6");
