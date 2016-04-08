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
	{ 0x7fdaa21, "remove_local_scheduler" },
	{ 0x71545ef5, "add_local_scheduler" },
	{ 0xd1edbd47, "quicksort" },
	{ 0x246523ec, "check_task_failure" },
	{ 0x962edfe, "livd" },
	{ 0x10dd6b8b, "initialize_dep" },
	{ 0x554f9bdf, "initialize_lists" },
	{ 0x101e5fa, "insert_on_list" },
	{ 0x137e346b, "copy_list" },
	{ 0x27e1a049, "printk" },
	{ 0x335f9f24, "list_is_feasible" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "39C8FE163BA0A112D70B2A0");
