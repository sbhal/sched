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
	{ 0xe0b1301e, "rt_sched_arch_concurrent" },
	{ 0xe07fb433, "presched_concurrent_generic" },
	{ 0x4fd3c696, "remove_global_scheduler" },
	{ 0x275f0d8, "add_global_scheduler" },
	{ 0x40d5678b, "_remove_task_global" },
	{ 0x898390a2, "task_pullable" },
	{ 0x47c7b0d2, "cpu_number" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "90C8412E19F871758E73C79");
