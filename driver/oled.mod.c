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
	{ 0x17634745, "module_layout" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x9660b5c3, "driver_unregister" },
	{ 0x26527e52, "spi_register_driver" },
	{ 0x593a99b, "init_timer_key" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xf53d8915, "arm_delay_ops" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x7d11c268, "jiffies" },
	{ 0x8834396c, "mod_timer" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x27e1a049, "printk" },
	{ 0xfe990052, "gpio_free" },
	{ 0xc996d097, "del_timer" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Csaak,oled-2.8*");
MODULE_ALIAS("of:N*T*Csaak,oled-2.8-font*");
MODULE_ALIAS("spi:oled-2.8");
MODULE_ALIAS("spi:oled-2.8-font");

MODULE_INFO(srcversion, "91E56EA170EC4634129C05A");
