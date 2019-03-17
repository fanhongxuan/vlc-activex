ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = \
	1.11 \
	-Wall \
	check-news \
	dist-xz \
	no-dist-gzip

DIST_SUBDIRS= share common npapi activex
SUBDIRS = common
if BUILD_NPAPI
SUBDIRS += npapi
endif
if BUILD_ACTIVEX
SUBDIRS += activex
endif

DISTCHECK_CONFIGURE_FLAGS=--without-gtk

EXTRA_DIST = \
	autogen.sh \
	extras/macosx/Info.plist.in \
	extras/macosx/build-package.sh \
	extras/macosx/build-plugin.sh \
	extras/macosx/build-vlc.sh \
	extras/macosx/create-universal-binary.sh

package-macosx-xpi:
	mkdir -p "$(top_builddir)/macosx-xpi/plugins"
	cp $(top_builddir)/src/install.rdf \
		"$(top_builddir)/macosx-xpi/"
	cp -R "$(top_builddir)/src/VLC Plugin.plugin" \
		"$(top_builddir)/macosx-xpi/plugins"
	cd "$(top_builddir)/macosx-xpi" && zip -r vlc-$(VERSION).xpi install.rdf plugins
