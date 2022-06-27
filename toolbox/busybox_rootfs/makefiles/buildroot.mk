download_path := download

# $1: url
# $2: output, the filename
define get_filename
	$(eval filename_splited := $(subst /, , $1))
	@echo $(filename_splited)
	$(eval filename_splited_len := $(words $(filename_splited)))
	$(eval res := $(word $(filename_splited_len), $(filename_splited)))
	$(eval $2 := $$(res))
endef


define check_file_with_sha1

endef


# $(1): buildroot url
# $(2): buildroot sha1
define setup_buildroot
	@rm -rf $(download_path)
	@mkdir $(download_path)
	$(call get_filename, $1, filename)
	@wget $1
	@echo -n $2
endef