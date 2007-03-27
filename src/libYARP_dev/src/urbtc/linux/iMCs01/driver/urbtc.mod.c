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
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xaa439674, "struct_module" },
	{ 0xf85bd710, "usb_register_dev" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xedd8bcce, "usb_get_dev" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x38646163, "kmem_cache_alloc" },
	{ 0xb549429b, "malloc_sizes" },
	{ 0x865edc9b, "finish_wait" },
	{ 0xc8f02aeb, "prepare_to_wait" },
	{ 0x4292364c, "schedule" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x96b27088, "__down_failed" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0x2cd3086, "__down_failed_trylock" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x6491cc9c, "usb_find_interface" },
	{ 0xe0b7c175, "usb_register_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0xfa6a61fe, "usb_put_dev" },
	{ 0xd2443b32, "usb_submit_urb" },
	{ 0xcb75e004, "usb_bulk_msg" },
	{ 0x833a467c, "usb_free_urb" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x2e61f689, "usb_buffer_alloc" },
	{ 0x93cda85, "usb_alloc_urb" },
	{ 0x625acc81, "__down_failed_interruptible" },
	{ 0xda4008e6, "cond_resched" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x225bf6fb, "usb_buffer_free" },
	{ 0xdf227c5b, "usb_kill_urb" },
	{ 0x1b7d4074, "printk" },
	{ 0x59968f3c, "__wake_up" },
	{ 0xcff53400, "kref_put" },
	{ 0x5dfe8f1a, "unlock_kernel" },
	{ 0xc560c341, "usb_deregister_dev" },
	{ 0x5568be43, "lock_kernel" },
	{ 0x32d7b675, "usb_deregister" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

MODULE_ALIAS("usb:v08DApFC00d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FF8p0001d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "C69A7FCAE375E92AF98C859");
