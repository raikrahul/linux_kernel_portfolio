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
	{ 0xb54d37ac, "alloc_pages_noprof" },
	{ 0xbd03ed67, "vmemmap_base" },
	{ 0x44decd6f, "hugetlb_optimize_vmemmap_key" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfed1e3bc, "kmalloc_caches" },
	{ 0x70db3fe4, "__kmalloc_cache_noprof" },
	{ 0xbd03ed67, "page_offset_base" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xdf157197, "__free_pages" },
	{ 0xbd03ed67, "phys_base" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xb54d37ac,
	0xbd03ed67,
	0x44decd6f,
	0xbd03ed67,
	0xfed1e3bc,
	0x70db3fe4,
	0xbd03ed67,
	0xcb8b6ec6,
	0xdf157197,
	0xbd03ed67,
	0xe8213e80,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"alloc_pages_noprof\0"
	"vmemmap_base\0"
	"hugetlb_optimize_vmemmap_key\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"page_offset_base\0"
	"kfree\0"
	"__free_pages\0"
	"phys_base\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C58E599126C2F310F4A5ED2");
