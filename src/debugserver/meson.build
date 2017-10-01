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
    'gstdebugserver',
    gstdebugger_sources,
    dependencies : gstdebugger_common_dep,
    install : true)
