object-model_source = src
object-model_configure_subdir = object-model
object-model_configure_depend = vpp-install
object-model_CPPFLAGS = $(call installed_includes_fn, vpp)
object-model_LDFLAGS = $(call installed_libs_fn, vpp)
