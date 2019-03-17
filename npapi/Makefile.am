###############################################################################
# Building the Mozilla plugin
###############################################################################

AUTOMAKE_OPTIONS = subdir-objects
EXTRA_DIST = package/npvlc_rc.rc.in package/npvlc.dll.manifest
MOSTLYCLEANFILES =
CLEANFILES = $(BUILT_SOURCES) \
	npapi-sdk/npapi.h \
	npapi-sdk/npfunctions.h \
	npapi-sdk/npruntime.h \
	npapi-sdk/nptypes.h

npvlcdir = $(libdir)/mozilla/plugins

AM_CPPFLAGS = $(LIBVLC_CFLAGS) -Inpapi-sdk $(MOZILLA_CFLAGS)

libvlcplugin_la_SOURCES = \
	common.h \
	locking.h \
	events.h \
	events.cpp \
	vlcshell.h \
	vlcshell.cpp \
	vlcplugin.h \
	vlcplugin_base.cpp \
	vlcplugin_base.h \
	npruntime/npolibvlc.cpp \
	npruntime/npolibvlc.h \
	npruntime/nporuntime.cpp \
	npruntime/nporuntime.h

libvlcplugin_la_DEPENDENCIES =
libvlcplugin_la_LIBADD = ../common/libvlcplugin_common.la $(LIBVLC_LIBS)
libvlcplugin_la_LDFLAGS = \
	-module -avoid-version \
	-no-undefined

if HAVE_WIN32
libvlcplugin_la_LDFLAGS += -Wc,--static -Wc,-static-libgcc
endif

npvlc_la_SOURCES = $(libvlcplugin_la_SOURCES)
npvlc_la_DEPENDENCIES = $(libvlcplugin_la_DEPENDENCIES)
npvlc_la_LIBADD = $(libvlcplugin_la_LIBADD)
npvlc_la_LDFLAGS = $(libvlcplugin_la_LDFLAGS)

npapi-sdk:
	svn export http://npapi-sdk.googlecode.com/svn/trunk/headers npapi-sdk-svn -r HEAD
	mv npapi-sdk-svn npapi-sdk

if HAVE_XCB

# Unix (x11) platforms
#
npvlc_LTLIBRARIES = libvlcplugin.la

if FETCH_NPAPI
$(libvlcplugin_la_OBJECTS): npapi-sdk
endif

AM_CPPFLAGS += -DXP_UNIX -DDATA_PATH=\"$(pkgdatadir)\" $(XCB_CFLAGS)

libvlcplugin_la_SOURCES += \
	vlcwindowless_xcb.cpp vlcwindowless_xcb.h \
	vlcwindowless_base.cpp vlcwindowless_base.h \
	npcontrol/npunix.cpp npcontrol/npcommon.cpp
libvlcplugin_la_LIBADD += $(MOZILLA_LIBS) $(XCB_LIBS)

if WITH_GTK
AM_CPPFLAGS += $(GTK_CFLAGS)

libvlcplugin_la_SOURCES += \
	vlcplugin_gtk.cpp vlcplugin_gtk.h
libvlcplugin_la_LIBADD += $(GTK_LIBS)

else # !WITH_GTK
libvlcplugin_la_SOURCES += \
	vlcplugin_xcb.cpp vlcplugin_xcb.h
endif # !USE_GTK

endif

if HAVE_WIN32

# Under Win32|Mac, Mozilla plugins need to be named NP******.DLL, but under Unix
# the common naming scheme is lib******plugin.so.
#
lib_LTLIBRARIES = npvlc.la

if FETCH_NPAPI
$(npvlc_la_OBJECTS): npapi-sdk
endif

AM_CPPFLAGS += -DXP_WIN -DXP_WIN32 \
	-DXPCOM_GLUE -DHAVE_CPP_MODERN_SPECIALIZE_TEMPLATE_SYNTAX

libvlcplugin_la_SOURCES += \
	vlcplugin_win.cpp vlcplugin_win.h \
	vlcwindowless_base.cpp vlcwindowless_base.h \
	vlcwindowless_win.cpp vlcwindowless_win.h \
	npcontrol/npwin.cpp npcontrol/npcommon.cpp

npvlc_la_DEPENDENCIES += $(DATA_npvlc_rc)
npvlc_la_LDFLAGS += -Wl,--kill-at -Wl,$(DATA_npvlc_rc)
npvlc_la_LIBADD += -lgdi32

DATA_npvlc_rc = $(noinst_DATA)
noinst_DATA = npvlc_rc.$(OBJEXT)

npvlc_rc.$(OBJEXT): package/npvlc_rc.rc
	$(WINDRES) --include-dir $(srcdir) -i $< -o $@

endif

if HAVE_DARWIN

if FETCH_NPAPI
$(npvlc_la_OBJECTS): npapi-sdk libvlcplugin_objc.la
$(libvlcplugin_objc_la_SOURCES): npapi-sdk
endif

# MacOS X
# Mac OS plugins need .r files that will be processed into rsrc files
#
lib_LTLIBRARIES = npvlc.la

AM_CPPFLAGS += -I. -I$(top_builddir) -c \
	-DXP_UNIX -DXP_MACOSX=1 \
	-DNO_X11=1 -DUSE_SYSTEM_CONSOLE=1 -pipe -fmessage-length=0

libvlcplugin_objc_la_SOURCES = \
	vlcplugin_mac.mm vlcplugin_mac.h
libvlcplugin_objc_la_LDFLAGS = -static
noinst_LTLIBRARIES = libvlcplugin_objc.la

libvlcplugin_la_SOURCES += \
	vlcwindowless_base.cpp vlcwindowless_base.h \
	vlcwindowless_mac.cpp vlcwindowless_mac.h \
	npcontrol/npmac.cpp npcontrol/npcommon.cpp
libvlcplugin_la_LIBADD += libvlcplugin_objc.la
libvlcplugin_la_LDFLAGS += \
	-bundle -Wl,-headerpad_max_install_names \
	-Wl,-framework,CoreFoundation -Wl,-framework,ApplicationServices \
	-Wl,-framework,QuartzCore, -Wl,-framework,AppKit, -shrext .dylib

noinst_DATA = VLC-Plugin.plugin
CLEANFILES += VLC-Plugin.plugin

VLC-Plugin.plugin: $(lib_LTLIBRARIES)
	rm -Rf "$(top_builddir)/$@"
	$(INSTALL) -d "$(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/lib"
	$(INSTALL) -d "$(top_builddir)/VLC-Plugin.plugin/Contents/Resources"
	$(INSTALL) $(top_srcdir)/share/pixmaps/macosx/*.png "$(top_builddir)/VLC-Plugin.plugin/Contents/Resources"
	ACTION="release-makefile" PRODUCT="VLC-Plugin.plugin" libvlc_dir=$(LIBVLC_PREFIX) src_dir=$(srcdir) build_dir=$(top_builddir) sh "$(top_srcdir)/extras/macosx/build-package.sh"
	$(INSTALL) "$(top_builddir)/extras/macosx/Info.plist" "$(top_builddir)/VLC-Plugin.plugin/Contents/Info.plist"
	xcrun plutil -convert binary1 "$(top_builddir)/VLC-Plugin.plugin/Contents/Info.plist"
	$(INSTALL) "$(top_builddir)/npapi/.libs/npvlc.dylib" "$(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/"
	mv $(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/npvlc.dylib "$(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/VLC-Plugin"
	chmod +x "$(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/VLC-Plugin"
	install_name_tool -change $(LIBVLC_PREFIX)/lib/libvlc.5.dylib @loader_path/lib/libvlc.5.dylib $(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/VLC-Plugin
	install_name_tool -change $(LIBVLC_PREFIX)/lib/libvlccore.8.dylib @loader_path/lib/libvlccore.8.dylib $(top_builddir)/VLC-Plugin.plugin/Contents/MacOS/VLC-Plugin
	find "$(top_builddir)/VLC-Plugin.plugin" -type d -exec chmod ugo+rx '{}' \;
	find "$(top_builddir)/VLC-Plugin.plugin" -type f -exec chmod ugo+r '{}' \;

endif # Mac
