#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xcf172cc1, "module_layout" },
	{ 0xa5b61d52, "rt_sched_arch_stw" },
	{ 0x7b90ce20, "presched_stw_generic" },
	{ 0x4fd3c696, "remove_global_scheduler" },
	{ 0x275f0d8, "add_global_scheduler" },
	{ 0x4cbbd171, "__bitmap_weight" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "47FFFEBC8CBD088B7F68148");
