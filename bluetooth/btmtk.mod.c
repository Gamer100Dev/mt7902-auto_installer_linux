#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.arch = MODULE_ARCH_INIT,
};

KSYMTAB_FUNC(btmtk_fw_get_filename, "_gpl", "");
KSYMTAB_FUNC(btmtk_setup_firmware_79xx, "_gpl", "");
KSYMTAB_FUNC(btmtk_setup_firmware, "_gpl", "");
KSYMTAB_FUNC(btmtk_set_bdaddr, "_gpl", "");
KSYMTAB_FUNC(btmtk_reset_sync, "_gpl", "");
KSYMTAB_FUNC(btmtk_register_coredump, "_gpl", "");
KSYMTAB_FUNC(btmtk_process_coredump, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_subsys_reset, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_recv_acl, "_gpl", "");
KSYMTAB_FUNC(alloc_mtk_intr_urb, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_resume, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_suspend, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_setup, "_gpl", "");
KSYMTAB_FUNC(btmtk_usb_shutdown, "_gpl", "");

MODULE_INFO(depends, "bluetooth");


MODULE_INFO(srcversion, "CDBDDC823AD88314A6658EA");
