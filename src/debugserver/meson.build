gstdebugger_sources = [
    'gstdebugserver.c',
    'gstdebugservertcp.c',
    'gstdebugserverlog.c',
    'gstdebugservermessage.c',
    'gstdebugservertypes.c',
    'gstdebugserverqe.c',
    'gstdebugservertopology.c',
    'gstdebugserverhooks.c',
    'gstdebugserverbuffer.c']

gstdebugger = shared_library(
    'gstdebugserver-@0@'.format(gst_apiversion),
    gstdebugger_sources,
    dependencies : gstdebugger_common_dep,
    install : true,
    install_dir : '@0@/gstreamer-@1@'.format(get_option('libdir'), gst_apiversion))
