#!/bin/sh
set -e

info()
{
    local green="\033[1;32m"
    local normal="\033[0m"
    echo "[${green}build${normal}] $1"
}

ARCH="x86_64"
MINIMAL_OSX_VERSION="10.6"
OSX_VERSION="10.9"
SDKROOT=`xcode-select -print-path`/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$OSX_VERSION.sdk
VERBOSE=no

usage()
{
cat << EOF
usage: $0 [options]

Build vlc in the current directory

OPTIONS:
   -h            Show some help
   -v            Be verbose
   -k <sdk>      Use the specified sdk (default: $SDKROOT)
   -a <arch>     Use the specified arch (default: $ARCH)
EOF

}

spushd()
{
    pushd "$1" > /dev/null
}

spopd()
{
    popd > /dev/null
}

while getopts "hvk:a:" OPTION
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         v)
             VERBOSE=yes
             ;;
         a)
             ARCH=$OPTARG
             ;;
         k)
             SDKROOT=$OPTARG
         ;;
     esac
done
shift $(($OPTIND - 1))

out="/dev/null"
if [ "$VERBOSE" = "yes" ]; then
   out="/dev/stdout"
fi

if [ "x$1" != "x" ]; then
    usage
    exit 1
fi

#
# Various initialization
#

info "Building VLC Web-Plugin for ${ARCH}"

spushd `dirname $0`/../../
npapiroot=`pwd`
spopd

builddir=`pwd`

info "Building in \"$builddir\""

export CC="xcrun clang -isysroot ${SDKROOT} -mmacosx-version-min=${MINIMAL_OSX_VERSION}"
export CXX="xcrun clang++ -isysroot ${SDKROOT} -mmacosx-version-min=${MINIMAL_OSX_VERSION}"
export OBJC="xcrun clang -isysroot ${SDKROOT} -mmacosx-version-min=${MINIMAL_OSX_VERSION}"
export OBJCXX="xcrun clang++ -isysroot ${SDKROOT} -mmacosx-version-min=${MINIMAL_OSX_VERSION}"
export LD="xcrun ld -syslibroot ${SDKROOT} -mmacosx-version-min=${MINIMAL_OSX_VERSION}"

export PATH="${npapiroot}/extras/macosx/vlc/extras/tools/build/bin:${npapiroot}/extras/macosx/vlc/contrib/${ARCH}-apple-darwin10/bin:$PATH"
export PKG_CONFIG_PATH="${npapiroot}/extras/macosx/vlc/${ARCH}-install/lib/pkgconfig"

spushd ${npapiroot}

if [ ! -e "configure" ]; then
info "Bootstrapping"
./autogen.sh
fi

info "Creating builddir and configuration"
mkdir -p ${ARCH}-build && cd ${ARCH}-build
../configure --build=${ARCH}-apple-darwin10

core_count=`sysctl -n machdep.cpu.core_count`
let jobs=$core_count+1

info "Compilation in $jobs jobs"

if [ "$VERBOSE" = "yes" ]; then
    make -j$jobs V=1
else
    make -j$jobs
fi

spopd

info "Build completed"
