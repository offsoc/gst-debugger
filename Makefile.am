ACLOCAL_AMFLAGS = -I build -I common/m4

SUBDIRS = src po

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

dist_noinst_SCRIPTS = autogen.sh

# Auto-generate the ChangeLog file from the git log on make dist
include $(top_srcdir)/build/dist-changelog.am

EXTRA_DIST = $(INTLTOOL_FILES)
DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update \
                 po/.intltool-merge-cache
