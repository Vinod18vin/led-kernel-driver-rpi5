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
	{ 0x92997ed8, "_printk" },
	{ 0x8293d6f5, "gpio_to_desc" },
	{ 0x2e03eeb, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x33b9c91f, "device_destroy" },
	{ 0x6775d5d3, "class_destroy" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x418c10ec, "__register_chrdev" },
	{ 0x59c02473, "class_create" },
	{ 0x150281a5, "device_create" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x8bba3669, "gpiod_direction_output_raw" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C5FD3249778165B0ECE120D");
