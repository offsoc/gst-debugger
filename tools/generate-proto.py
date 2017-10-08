from subprocess import call
import os
import sys

if len(sys.argv) != 3:
    raise Exception('Usage: python3 generate-proto.py <common_dir> <debugger_dir>')

call([
    'protoc',
    '--proto_path=' + sys.argv[1],
    '--cpp_out=' + sys.argv[2],
    os.path.join(sys.argv[1], 'gstdebugger.proto')])

call([
    'protoc-c',
    '--proto_path=' + sys.argv[1],
    '--c_out=' + sys.argv[1],
    os.path.join(sys.argv[1], 'gstdebugger.proto')])
