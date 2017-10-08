install_headers_dir = join_paths('gstdebugger-@0@'.format(apiversion), 'gst', 'debugger')

gstdebugger_common_sources = [
    'gst-utils.c',
    'protocol-utils.c',
    'gstdebugger.pb-c.c',
    'serializer.c']

gstdebugger_common_headers = [
    'common.h',
    'gst-utils.h',
    'protocol-utils.h',
    'serializer.h']

gstdebugger_common_deps = [
    gst_dep,
    gio_dep,
    libprotobuf_c_dep]

gstdebugger_common_lib = shared_library(
    'gstdebugger-common-@0@'.format(apiversion),
    gstdebugger_common_sources,
    dependencies : gstdebugger_common_deps,
    install : true)

pkgconf = configuration_data()

pkgconf.set('prefix', get_option('prefix'))
pkgconf.set('exec_prefix', '${prefix}')
pkgconf.set('libdir', '${prefix}/@0@'.format(get_option('libdir')))
pkgconf.set('includedir', '${prefix}/@0@'.format(get_option('includedir')))
pkgconf.set('GST_API_VERSION', gst_apiversion)
pkgconf.set('GSTREAMER_VERSION', gstreamer_version)
pkgconf.set('PROTOBUF_C_VERSION', protobuf_c_version)
pkgconf.set('GIO_VERSION', glib_version)
pkgconf.set('API_VERSION', apiversion)
pkgconf.set('VERSION', meson.project_version())

pkg_install_dir = '@0@/pkgconfig'.format(get_option('libdir'))

configure_file(
    input : 'libgstdebugger-common.pc.in',
    output : 'libgstdebugger-common-@0@.pc'.format(apiversion),
    configuration : pkgconf,
    install_dir : pkg_install_dir)

install_headers(
    gstdebugger_common_headers,
    subdir : install_headers_dir)

gstdebugger_common_dep = declare_dependency(
    link_with : gstdebugger_common_lib,
    include_directories : include_directories('..'),
    dependencies : gstdebugger_common_deps)
