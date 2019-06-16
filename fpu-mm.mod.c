#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xb6aa582b, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x4983252f, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x329ee78b, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x52296593, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x3ba55f6b, __VMLINUX_SYMBOL_STR(pci_release_region) },
	{ 0x68e2f221, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0x69a358a6, __VMLINUX_SYMBOL_STR(iomem_resource) },
	{ 0xdc48c03a, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0xe071a332, __VMLINUX_SYMBOL_STR(dev_printk) },
	{ 0x5812b3a7, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xb4a495d0, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xd244ac3d, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xb077c45d, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0xff7559e4, __VMLINUX_SYMBOL_STR(ioport_resource) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
	{ 0x9b7e2342, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x2072ee9b, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x6d664ef4, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x7c61340c, __VMLINUX_SYMBOL_STR(__release_region) },
	{ 0xb300fe35, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x4289132b, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x67f7403e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x883c8412, __VMLINUX_SYMBOL_STR(remap_pfn_range) },
	{ 0xf2493bc6, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x167b3662, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0x6b7b80cb, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x432464b4, __VMLINUX_SYMBOL_STR(pci_enable_msi_block) },
	{ 0xeffec5a0, __VMLINUX_SYMBOL_STR(pci_iomap) },
	{ 0xcd615843, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xb93c77fc, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x46ae721c, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0xe3b2469c, __VMLINUX_SYMBOL_STR(pci_request_region) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2FC00A393F425E86294AD53");
