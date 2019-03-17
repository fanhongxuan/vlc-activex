#!/bin/sh
set -e

info()
{
    local green="\033[1;32m"
    local normal="\033[0m"
    echo "[${green}build${normal}] $1"
}

BUILDBOT=no

usage()
{
cat << EOF
usage: $0 [options]

OPTIONS
   -h            Show this help
   -b            Enable buildbot mode (if you don't have a window server)
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

while getopts "hb" OPTION
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         b)
             BUILDBOT=yes
             ;;
     esac
done

out="/dev/null"

npapiroot=`dirname $0`/../..

UBROOT="${npapiroot}/VLC Plugin.plugin"
DMGFOLDERNAME="VLC Browser Plug-in for Mac"
DMGITEMNAME="VLC-webplugin-REPLACEWITHVERSION"

info "checking for distributable binary package"

spushd ${npapiroot}
if [ ! -e "${UBROOT}" ]; then
    info "Universal Binary not found for distribution, creating..."
    ./extras/macosx/create-universal-binary.sh
fi

info "Collecting items"
mkdir -p "${DMGFOLDERNAME}"
cp -R "${UBROOT}" "${DMGFOLDERNAME}"
cp NEWS AUTHORS COPYING "${DMGFOLDERNAME}"
spushd "${DMGFOLDERNAME}"
mv NEWS NEWS.txt
mv AUTHORS AUTHORS.txt
mv COPYING COPYING.txt
spopd
ln -s "/Library/Internet Plug-Ins" "${DMGFOLDERNAME}/Internet Plug-Ins"
rm -f ${DMGITEMNAME}-rw.dmg

info "Creating disk-image"
hdiutil create -srcfolder ${npapiroot}/"${DMGFOLDERNAME}" "${npapiroot}/${DMGITEMNAME}-rw.dmg" -scrub -format UDRW
mkdir -p ./mount

if [ "$BUILDBOT" = "no" ]; then
    info "Moving file icons around"
    hdiutil attach -readwrite -noverify -noautoopen -mountRoot ./mount ${DMGITEMNAME}-rw.dmg
    osascript "${npapiroot}"/extras/macosx/dmg_setup.scpt "${DMGFOLDERNAME}"
    hdiutil detach ./mount/"${DMGFOLDERNAME}"
fi

info "Compressing disk-image"
rm -f ${DMGITEMNAME}.dmg
hdiutil convert "${npapiroot}/${DMGITEMNAME}-rw.dmg" -format UDBZ -o "${npapiroot}/${DMGITEMNAME}.dmg"
rm -f ${DMGITEMNAME}-rw.dmg
rm -rf "${DMGFOLDERNAME}"

spopd

info "Disk-image created"
