# Generate proto source files
run_target('generateproto',
    command : [
        python.find_python(),
        join_paths(meson.source_root(), 'tools/generate-proto.py'),
        join_paths(meson.current_source_dir(), 'common'),
        join_paths(meson.current_source_dir(), 'gst-debugger')])

subdir('common')
subdir('debugserver')
subdir('gst-debugger')
