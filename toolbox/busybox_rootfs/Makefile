ROOTFS_OUTPUT := ./output
ROOTFS_BIN := $(ROOTFS_OUTPUT)/bin
BUILDROOT_URL := https://buildroot.org/downloads/buildroot-2022.02.3.tar.gz
BUILDROOT_SHA1 := "a3d3f83d009e99c3d3476eb6ca99f8c52ca18f69  buildroot-2022.02.3.tar.gz"



include makefiles/setuprootfs.mk
include makefiles/busybox.mk
include makefiles/buildroot.mk


.PHONY: all
all:
	$(call setup_rootfs, $(ROOTFS_OUTPUT))
	$(call setup_busybox, $(ROOTFS_BIN))
	$(call setup_buildroot, $(BUILDROOT_URL), $(BUILDROOT_SHA1))
