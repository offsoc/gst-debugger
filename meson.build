project('gst-debugger', 'c', 'cpp',
  license: 'GPL',
  version: '0.90.0',
  meson_version: '>= 0.40.1',
  default_options: [
    'cpp_std=c++11',
    'warning_level=1',
  ],
)

# Modules
gnome = import('gnome')

# Dependencies
glib_version = '>=2.41.1'
gio_dep = dependency('gio-2.0', version : glib_version)
gmoduledep = dependency('gmodule-2.0', version : glib_version)
gobjectdep = dependency('gobject-2.0', version : glib_version)
gst_dep = dependency('gstreamer-1.0', version : '>=1.8.0')
gstmm_dep = dependency('gstreamermm-1.0')
gtkmm_dep = dependency('gtkmm-3.0', version : '>=3.14.0')
x11_dep = dependency('x11')
gvc_dep = dependency('libgvc', version : '>=2.38')
libprotobuf_c_dep = dependency('libprotobuf-c', version : '>=1.1.1')
protobuf_dep = dependency('protobuf', version : '>=2.6.1')
boost_dep = dependency('boost', modules : ['filesystem', 'system'], version : '>=1.55')
thread_dep = dependency('threads')

# Dependency apps
protoc_c = find_program('protoc-c')

# Configuration file
conf = configuration_data()
conf.set_quoted('PACKAGE_NAME', 'gnome-paint')
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('GST_DEBUGGER_LOCALEDIR', join_paths(get_option('prefix'), get_option('datadir'), 'locale'))
conf.set('GETTEXT_PACKAGE', 'PACKAGE_NAME')
configure_file(output : 'config.h',
               configuration : conf)
config_inc_dir = include_directories('.')

# Subdirectories
subdir('src')
subdir('po')
