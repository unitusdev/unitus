package=mman-win32
$(package)_version=b7ec370
$(package)_download_path=https://github.com/witwall/mman-win32/tarball/$($(package)_version)/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=6f94db28ddf30711c7b227e97c5142f72f77aca2c5cc034a7d012db242cc2f7b

define $(package)_set_vars
$(package)_build_opts= CC="$($(package)_cc)"
$(package)_build_opts+=CFLAGS="$($(package)_cflags) $($(package)_cppflags) -fPIC"
$(package)_build_opts+=RANLIB="$($(package)_ranlib)"
$(package)_build_opts+=AR="$($(package)_ar)"
$(package)_build_opts_darwin+=AR="$($(package)_libtool)"
$(package)_build_opts_darwin+=ARFLAGS="-o"
endef

define $(package)_config_cmds
  /bin/sh $($(package)_extract_dir)/configure --cross-prefix=$(host_prefix)- --prefix=$($(package)_staging_prefix_dir) --libdir=$($(package)_staging_prefix_dir)/lib --incdir=$($(package)_staging_prefix_dir)/include/sys
endef

define $(package)_build_cmds
  $(MAKE) $($(package)_build_opts)
endef

define $(package)_stage_cmds
  $(MAKE) install
endef

