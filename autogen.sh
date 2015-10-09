#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/configure.ac) || {
        echo "**Error**: Directory "\`$srcdir\'" does not look like the top-level project directory"
        exit 1
}

# Make sure we have common
if test ! -f common/gst-autogen.sh;
then
  echo "+ Setting up common submodule"
  git submodule init
fi
git submodule update

# source helper functions
if test ! -f common/gst-autogen.sh;
then
  echo There is something wrong with your source tree.
  echo You are missing common/gst-autogen.sh
  exit 1
fi
. common/gst-autogen.sh

PKG_NAME=`autoconf --trace "AC_INIT:$1" "$srcdir/configure.ac"`

if [ "$#" = 0 -a "x$NOCONFIGURE" = "x" ]; then
        echo "**Warning**: I am going to run \`configure' with no arguments." >&2
        echo "If you wish to pass any to it, please specify them on the" >&2
        echo \`$0\'" command line." >&2
        echo "" >&2
fi

set -x

aclocal --install || exit 1
intltoolize --force --copy --automake || exit 1
autoreconf --verbose --force --install -Wno-portability || exit 1
automake --add-missing --copy >/dev/null 2>&1

if [ "$NOCONFIGURE" = "" ]; then
        $srcdir/configure --enable-maintainer-mode "$@" || exit 1

        if [ "$1" = "--help" ]; then exit 0 else
                echo "Now type \`make\' to compile $PKG_NAME" || exit 1
        fi
else
        echo "Skipping configure process."
fi

set +x
