dnl Autoconf settings for npapi-vlc

AC_COPYRIGHT([Copyright 2002-2015 VLC authors and VideoLAN])

AC_INIT(npapi-vlc, 2.2.1)
VERSION_MAJOR="2"
AC_SUBST(VERSION_MAJOR)
VERSION_MINOR="2"
AC_SUBST(VERSION_MINOR)
VERSION_REVISION="1"
AC_SUBST(VERSION_REVISION)
VERSION_EXTRA=""
AC_SUBST(VERSION_EXTRA)
dnl Win32 need s a numerical version_extra.
case $( echo ${VERSION_EXTRA}|wc -m ) in
       "1") VERSION_EXTRA_RC="0";;
       "2") VERSION_EXTRA_RC=$( echo ${VERSION_EXTRA}|tr "abcdefghi" "123456789") ;;
       *) if test "${VERSION_EXTRA}" = "-git"; then VERSION_EXTRA_RC="666"; else VERSION_EXTRA_RC="99"; fi
esac
AC_SUBST(VERSION_EXTRA_RC)
COPYRIGHT_YEARS="2002-2015"
AC_SUBST(COPYRIGHT_YEARS)
COPYRIGHT_MESSAGE="Copyright © ${COPYRIGHT_YEARS} VLC authors and VideoLAN"
AC_SUBST(COPYRIGHT_MESSAGE)

AC_CONFIG_SRCDIR(npapi/vlcshell.cpp)
AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_MACRO_DIR(m4)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_PRESERVE_HELP_ORDER

AM_INIT_AUTOMAKE(tar-ustar color-tests)
AC_CONFIG_HEADERS([config.h])

# Disable with "./configure --disable-silent-rules" or "make V=1"
AM_SILENT_RULES([yes])

dnl
dnl Check for tools
dnl
AC_PROG_CC_C99
AC_USE_SYSTEM_EXTENSIONS
AC_DEFINE([_FORTIFY_SOURCE], 2, [Define to '2' to get glibc warnings.])
AC_PROG_CXX
AC_PROG_OBJCXX

dnl override platform specific check for dependent libraries
dnl otherwise libtool linking of shared libraries will
dnl fail on anything other than pass_all.
AC_CACHE_VAL(lt_cv_deplibs_check_method,
    [lt_cv_deplibs_check_method=pass_all])

AC_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG
AC_CHECK_TOOL(DLLTOOL, dlltool, :)

dnl Check for compiler properties
AC_C_CONST

WIDLFLAGS=""
dnl
dnl  Check the operating system
dnl
case "${host_os}" in
  darwin*)
    SYS=darwin

    dnl Force gcc "-arch" flag
    ARCH_flag=""
    case "${host}" in
      i*86*)
        ARCH_flag="-arch i386"
      ;;
      ppc64*)
        ARCH_flag="-arch ppc64"
      ;;
      ppc*)
        ARCH_flag="-arch ppc"
      ;;
      x86_64*)
        ARCH_flag="-arch x86_64"
      ;;
    esac

    CFLAGS="${CFLAGS} -fobjc-exceptions -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    CXXFLAGS="${CXXFLAGS} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    OBJCXXFLAGS="${OBJCXXFLAGS} -fobjc-exceptions -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    CPPFLAGS="${CPPFLAGS} ${ARCH_flag}"
    LDFLAGS="${LDFLAGS} -Wl,-headerpad_max_install_names ${ARCH_flag}"
    ;;
  *mingw32*)
    SYS=mingw32
    AC_CHECK_TOOL(WINDRES, windres, :)
    AC_DEFINE([_WIN32_WINNT], 0x0501, [Define to '0x0501' for Windows XP APIs.])
    AC_DEFINE([_WIN32_IE], 0x0600, [Define to '0x0600' for IE6.0 APIs.])
    AC_DEFINE([_WIN32], 1, [Define to 1 if targetting Windows.])
    CC="$CC -static-libgcc"
    CXX="$CXX -static-libgcc -static-libstdc++"
    ;;
esac
AM_CONDITIONAL(HAVE_DARWIN, test "${SYS}" = "darwin")
AM_CONDITIONAL(HAVE_WIN32, test "${SYS}" = "mingw32")

LT_INIT

dnl
dnl Check widl/midl tools for win32
AS_IF([test "${SYS}" = "mingw32"],[
    AC_CHECK_PROGS(MIDL, [midl], no)
    AC_CHECK_TOOL(WIDL, [widl], no)

    case "${host}" in
        amd64*|x86_64*)
            WIDLFLAGS=" -m64"
        ;;
        *)
            WIDLFLAGS=" -m32"
        ;;
    esac
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADER(ole2.h,,[AC_MSG_ERROR([required OLE header ole2.h is missing from your system])])
    AC_CHECK_HEADER(olectl.h,,[AC_MSG_ERROR([required OLE header olectl.h is missing from your system])])
    AC_CHECK_HEADERS(objsafe.h,,,[#include <ole2.h>])
    AC_LANG_POP(C++)
],[
    MIDL="no"
    WIDL="no"
])

AC_SUBST(WIDLFLAGS)
AM_CONDITIONAL(HAS_MIDL_COMPILER, test "${MIDL}" != "no")
AM_CONDITIONAL(HAS_WIDL_COMPILER, test "${WIDL}" != "no")

dnl
dnl Buggy glibc prevention. Purposedly not cached.
dnl See sourceware.org bugs 5058 and 5443.
dnl Ubuntu alone has 20 bug numbers for this...
dnl
AC_MSG_CHECKING(for buggy GNU/libc versions)
AC_PREPROC_IFELSE([AC_LANG_SOURCE([
#include <limits.h>
#if defined (__GLIBC__) && (__GLIBC__ == 2) \
  && (__GLIBC_MINOR__ >= 5) && (__GLIBC_MINOR__ <= 7)
# error GNU/libc with dcgettext killer bug!
#endif
])], [
  AC_MSG_RESULT([not present])
], [
  AC_MSG_RESULT([found])
  AC_MSG_ERROR([Buggy GNU/libc (version 2.5 - 2.7) present. VLC would crash; there is no viable
work-around for this. Check with your distribution vendor on how to update the
glibc run-time.])
])

dnl
dnl libraries checks
AC_PATH_XTRA
PKG_CHECK_MODULES([LIBVLC], [libvlc >= 2.0.0], [
    LIBVLC_PREFIX=`pkg-config --variable=prefix libvlc`
    AC_SUBST(LIBVLC_PREFIX)])


dnl
dnl ActiveX
AC_ARG_ENABLE(activex, AS_HELP_STRING([--enable-activex], [ActiveX plugin [default=yes (on Windows)]]))
AM_CONDITIONAL(BUILD_ACTIVEX, test "${SYS}" = "mingw32" -a "${enable_activex}" != "no")


dnl
dnl NPAPI headers
AC_ARG_ENABLE(npapi, AS_HELP_STRING([--enable-npapi], [NPAPI/Mozilla plugin [default=yes]]))
AC_ARG_VAR([MOZILLA_CFLAGS], [C compiler flags for Mozilla, overriding pkg-config])
AC_ARG_VAR([MOZILLA_LIBS], [linker flags for Mozilla, overriding pkg-config])

AS_IF([test "x${enable_npapi}" != x"no"],[

AC_LANG_PUSH(C++)
AC_MSG_CHECKING([Mozilla headers])

dnl pkg-config
dnl As we want to do a loop due to the number of name possible for the .pc
dnl we can't use the pkg-config macros.
  for i in "${with_mozilla_pkg}" npapi-sdk seamonkey-plugin iceape-plugin xulrunner-plugin firefox-plugin iceweasel-plugin mozilla-plugin libxul; do
    if test -n "$i"; then
      echo "Trying to find $i package" >&AS_MESSAGE_LOG_FD
      AS_IF([test $i = npapi-sdk], [moz_ver=0.27], [moz_ver=1.9.2])
      AC_MSG_CHECKING([for $i >= $moz_ver])
      if $PKG_CONFIG --exists --print-errors "$i >= $moz_ver" 2>&AS_MESSAGE_LOG_FD
      then
        echo "Using $i pkg-config package." >&AS_MESSAGE_LOG_FD
        MOZILLA_CFLAGS=$(eval $PKG_CONFIG --cflags "$i" )
        MOZILLA_LIBS=$(eval $PKG_CONFIG --libs "$i" )
        AC_MSG_RESULT([yes])
        break
      else
        AC_MSG_RESULT([no])
      fi
    fi
  done

CPPFLAGS_save="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${MOZILLA_CFLAGS} -Inpapi/npapi-sdk/"

AC_CHECK_HEADERS(npapi.h npruntime.h, [
    dnl if found
    have_headers="true"
  ],[
    dnl if not found
    AC_MSG_WARN([Please install the Mozilla development tools, required headers were not found.])
    have_headers="false"
  ], [
    #include <stdint.h>
    #if HAVE_NPAPI_H
     #include <npapi.h>
    #endif
  ]
)
AM_CONDITIONAL(FETCH_NPAPI, [ test "${have_headers}" != "true" ])

dnl
dnl check the const-ness of GetMIMEDesc, since it changed in latest npapi headers
AC_MSG_CHECKING([if NP_GetMIMEDescription() returns const])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
   #ifdef WIN32
   # define XP_WIN 1
   #else
   # ifdef __APPLE__
   #  define XP_MACOSX 1
   # endif
   # define XP_UNIX 1
   #endif
   #include <stdint.h>
   #include <npapi.h>
   #include <npfunctions.h>
  char *NP_GetMIMEDescription(void);
])],[
  AC_MSG_RESULT(no)
  AC_DEFINE(NPP_GET_MIME_CONST, [], [Whether NP_GetMIMEDescription returns const])
],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(NPP_GET_MIME_CONST, [const], [Whether NP_GetMIMEDescription returns const])
])

CPPFLAGS="${CPPFLAGS_save}"

AC_LANG_POP(C++)

])
dnl end of NPAPI
AM_CONDITIONAL(BUILD_NPAPI, test "${enable_npapi}" != "no")

dnl
dnl GTK+ for linux toolbar
AC_ARG_WITH([gtk], AS_HELP_STRING([--with-gtk], [Build the GTK+ toolbars in NPAPI plugin [default=auto]]),, [with_gtk=yes])
gtk_found=no
AS_IF([ test "${SYS}" != "mingw32" -a "${SYS}" != "darwin" ], [
  PKG_CHECK_MODULES(XCB, [xcb x11-xcb],[xcb_found=yes], [
     AC_MSG_ERROR([Please install the libxcb and x11-xcb development files])
  ])
  AS_IF([ test "x$with_gtk" != "xno" ],
    [
       PKG_CHECK_MODULES(GTK, [gtk+-2.0], [gtk_found=yes])
    ])
])
AS_IF([ test "x$gtk_found" = "xyes" ],
    AC_DEFINE([USE_GTK], [1], [Define to 1 if using GTK+]))
AM_CONDITIONAL(WITH_GTK, [ test "x$gtk_found" = "xyes" ])
AM_CONDITIONAL(HAVE_XCB, [ test "x$xcb_found" = "xyes" ])

dnl
dnl final flags for ActiveX
AM_COND_IF([HAVE_WIN32], [
  ACTIVEX_CPPFLAGS="${CPPFLAGS} -DUNICODE -D_UNICODE -D_MIDL_USE_GUIDDEF_"
  ACTIVEX_CXXFLAGS="${CXXFLAGS} -fno-exceptions"

  AC_ARG_VAR([ACTIVEX_LIBS], [linker flags for ActiveX])
  ACTIVEX_LIBS="${ACTIVEX_LIBS} -lole32 -loleaut32 -luuid -lshlwapi -lgdi32"
])

AC_CONFIG_FILES([
  Makefile
  share/Makefile
  common/Makefile
  npapi/Makefile
  activex/Makefile
  npapi/package/install.js
  npapi/package/install.rdf
  npapi/package/manifest.json
])

AM_COND_IF([HAVE_WIN32], [
  AC_CONFIG_FILES([
    npapi/package/npvlc_rc.rc
    activex/axvlc.inf
    activex/axvlc_rc.rc
  ])
])
AM_COND_IF([HAVE_DARWIN], [
  AC_CONFIG_FILES([
    npapi/package/vlc.r
    extras/macosx/Info.plist
  ])
])
AC_OUTPUT
