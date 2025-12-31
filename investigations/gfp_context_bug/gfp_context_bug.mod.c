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
	{ 0x4ff89cd9, "pcpu_hot" },
	{ 0xe1e1f979, "_raw_spin_lock_irqsave" },
	{ 0xb54d37ac, "alloc_pages_noprof" },
	{ 0xbd03ed67, "vmemmap_base" },
	{ 0xdf157197, "__free_pages" },
	{ 0x81a1a811, "_raw_spin_unlock_irqrestore" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x4ff89cd9,
	0xe1e1f979,
	0xb54d37ac,
	0xbd03ed67,
	0xdf157197,
	0x81a1a811,
	0xe8213e80,
	0xd272d446,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"pcpu_hot\0"
	"_raw_spin_lock_irqsave\0"
	"alloc_pages_noprof\0"
	"vmemmap_base\0"
	"__free_pages\0"
	"_raw_spin_unlock_irqrestore\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "446EE4236D11A6F00A37A80");
