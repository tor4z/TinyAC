download_path := download

# $1: url
# $2: output, the filename
define get_filename
	$(eval filename_splited := $(subst /, , $1))
	$(eval filename_splited_len := $(words $(filename_splited)))
	$(eval res := $(word $(filename_splited_len), $(filename_splited)))
	$(eval $2 := $(strip $(res)))
endef


# $1: filename
# $1: sha1 value
define check_file_with_sha1
	$(eval sha1_string := "$2  $1")
	$(eval sha1_file := "$1.sha1")
	$(eval sha1_file := $(strip $(sha1_file)))
	@echo "SHA1 value: $2"
	@echo -n $(sha1_string) > $(download_path)/$(sha1_file)
	@echo "Saved SHA1 info to $(sha1_file)"
	cd $(download_path) && sha1sum -c $(sha1_file)
	@-rm -f $(download_path)/$(sha1_file)
endef


# $1: tar.gz filename
# $2: output
define get_uncompressed_filename
	$(eval $2 := $(basename $(basename $1)))
endef


# $1: buildroot url
# $2: buildroot sha1
define setup_buildroot
	@rm -rf $(download_path)
	@mkdir $(download_path)
	$(call get_filename, $1, filename)
	@cd $(download_path) && wget $1
	$(call check_file_with_sha1,$(filename),$2)
	@echo "Uncompressing $(filename)"
	@cd $(download_path) && tar xf $(filename)
	@echo $(call get_uncompressed_filename,$(filename),buildroot_dirname)
	@cp -r $(download_path)/$(buildroot_dirname)/system/skeleton/* $(ROOTFS_OUTPUT)
endef
