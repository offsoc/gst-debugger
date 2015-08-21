ACLOCAL_AMFLAGS = -I build -I common/m4

SUBDIRS = src

dist_noinst_SCRIPTS = autogen.sh

# Auto-generate the ChangeLog file from the git log on make dist
include $(top_srcdir)/build/dist-changelog.am
