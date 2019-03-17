AM_CPPFLAGS = $(LIBVLC_CFLAGS)

libvlcplugin_common_la_SOURCES = \
	position.h \
	vlc_player_options.h \
	vlc_player.cpp vlc_player.h
if HAVE_WIN32
libvlcplugin_common_la_SOURCES += \
	win32_fullscreen.cpp win32_fullscreen.h \
	win32_vlcwnd.cpp win32_vlcwnd.h
endif
libvlcplugin_common_la_LDFLAGS = -static

noinst_LTLIBRARIES = libvlcplugin_common.la
