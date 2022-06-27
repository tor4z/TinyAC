busybox_path := $(shell which busybox)

define setup_busybox
	@echo "copy busybox to $(1)"
	@cp $(busybox_path) $(1)

	@echo "install busybox"
	@$(1)/busybox --install $(1)
endef
