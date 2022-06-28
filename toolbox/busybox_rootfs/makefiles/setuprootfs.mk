# $1 root path
define setup_rootfs
	@echo "make rootfs at $1"
	@-rm -rf $1
	@mkdir $1

	@echo "make /bin dir"
	@mkdir $1/bin
endef


define make_pkg
	@cd $(ROOTFS_OUTPUT) && tar cf $(TARGET_PKG_NAME) ./*
endef
