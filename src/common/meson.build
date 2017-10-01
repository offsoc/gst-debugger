protoc_c_gen = generator(protoc_c,
    output    : ['@BASENAME@.pb-c.c', '@BASENAME@.pb-c.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@', '--c_out=@BUILD_DIR@', '@INPUT@'])
protoc_c_gen_src = protoc_c_gen.process('gstdebugger.proto')

generate_c_proto = custom_target('generate_proto',
   output : ['gstdebugger.pb-c.h'],
    input : 'gstdebugger.proto',
    command : [
        protoc_c,
        '--proto_path=' + meson.current_source_dir(),
        '--c_out=' + meson.current_source_dir(),
        join_paths(meson.current_source_dir(), 'gstdebugger.proto')])

gstdebugger_common_sources = [
    'gst-utils.c',
    'protocol-utils.c',
    'serializer.c']

gstdebugger_common_deps = [
    gio_dep,
    gst_dep,
    libprotobuf_c_dep]

gstdebugger_common_lib = static_library(
    'gstdebugger-common',
    gstdebugger_common_sources,
    generate_c_proto,
    protoc_c_gen_src,
    dependencies : gstdebugger_common_deps,
    install : true)

gstdebugger_common_dep = declare_dependency(
    link_with : gstdebugger_common_lib,
    include_directories : include_directories('..'),
    dependencies : gstdebugger_common_deps)
