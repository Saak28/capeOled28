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
	{ 0xb1755cf6, "spi_bus_type" },
	{ 0xcf169e44, "fb_sys_read" },
	{ 0x9660b5c3, "driver_unregister" },
	{ 0x26527e52, "spi_register_driver" },
	{ 0xbcfef8e7, "mutex_unlock" },
	{ 0x8835a9b7, "mutex_lock" },
	{ 0xf53d8915, "arm_delay_ops" },
	{ 0x86c8ad2e, "malloc_sizes" },
	{ 0x9b2fa56b, "register_framebuffer" },
	{ 0x403f9529, "gpio_request_one" },
	{ 0x3757c9b, "of_get_named_gpio_flags" },
	{ 0x99e50640, "__mutex_init" },
	{ 0x7a890c8, "fb_alloc_cmap" },
	{ 0x4be8ef0a, "fb_deferred_io_init" },
	{ 0x9d669763, "memcpy" },
	{ 0x8758668a, "kmem_cache_alloc_trace" },
	{ 0x1b3022e9, "framebuffer_alloc" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xe9525093, "dev_warn" },
	{ 0x782011c7, "devm_pinctrl_put" },
	{ 0x1eac9fb, "pinctrl_select_state" },
	{ 0x3f91b9ef, "pinctrl_lookup_state" },
	{ 0x13371c7b, "devm_pinctrl_get" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x4c071ecf, "sys_fillrect" },
	{ 0x2f4fcf5a, "sys_copyarea" },
	{ 0x1113bd, "sys_imageblit" },
	{ 0x9da01691, "schedule_delayed_work" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27e1a049, "printk" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x331013da, "spi_sync" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0xfe990052, "gpio_free" },
	{ 0x8fc8c71c, "framebuffer_release" },
	{ 0x999e8297, "vfree" },
	{ 0x37a0cba, "kfree" },
	{ 0x98b71c6, "fb_dealloc_cmap" },
	{ 0x15057616, "unregister_framebuffer" },
	{ 0x8ffa9e07, "dev_set_drvdata" },
	{ 0x3382adf, "dev_get_drvdata" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Csaak,oled-2.8*");
MODULE_ALIAS("of:N*T*Csaak,oled-2.8-font*");
MODULE_ALIAS("spi:saak,oled-2.8");
MODULE_ALIAS("spi:saak,oled-2.8-font");

MODULE_INFO(srcversion, "49C6782A0F6C73CE35319B9");
