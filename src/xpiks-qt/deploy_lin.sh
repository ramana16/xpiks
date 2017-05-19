#!/bin/bash

APP_NAME=xpiks-qt
VERSION="1.4.2"
APPDIR_NAME="Xpiks"

STAGING_DIR="./xpiks-qt.AppDir"

DEPLOY_TOOL=linuxdeployqt

APP_PROFILE=Release
LIBS_PROFILE=release

echo "------------------------------"

pushd ../build-xpiks-qt-*${APP_PROFILE}

# clear out any old data
echo "Wiping old data"
rm -v -rf "${STAGING_DIR}"

mkdir -p $STAGING_DIR

export PATH=~/bin:~/Qt5.6.2/5.6/gcc_64/bin:$PATH

cp -v ./$APP_NAME $STAGING_DIR/

cp -v -r ../xpiks-qt/deps/* $STAGING_DIR/
rm -v $STAGING_DIR/translations/*.ts
#cp -v ../xpiks-qt/debian/xpiks.desktop $STAGING_DIR
cp -v ../xpiks-qt/debian/xpiks.png $STAGING_DIR

cat << EOF > "$STAGING_DIR/$APP_NAME.desktop"
[Desktop Entry]
Type=Application
Name=$APP_NAME
GenericName=Xpiks
Exec=./AppRun %F
Icon=xpiks
Comment=Cross-platform (X) Photo Keywording Software
Terminal=true
StartupNotify=true
Encoding=UTF-8
Categories=Graphics;Photography;
EOF

chmod +x "$STAGING_DIR/$APP_NAME.desktop"

mkdir $STAGING_DIR/ac_sources
mv $STAGING_DIR/en_wordlist.tsv $STAGING_DIR/ac_sources/

echo "Working in directory: $(pwd)"

SRC_ROOT=../xpiks-qt/

DEPLOY_COMMAND="$DEPLOY_TOOL $STAGING_DIR/$APP_NAME -verbose=3 -no-strip -always-overwrite -qmldir=$SRC_ROOT -qmldir=$SRC_ROOT/Components/ -qmldir=$SRC_ROOT/Constants/ -qmldir=$SRC_ROOT/Dialogs/ -qmldir=$SRC_ROOT/StyledControls/ -qmldir=$SRC_ROOT/StackViews/ -qmldir=$SRC_ROOT/CollapserTabs/"

LD_LIBRARY_PATH=../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -bundle-non-qt-libs 
# bug in the linuxdeployqt - need to run twice (facepalm.jpg)
LD_LIBRARY_PATH=../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -bundle-non-qt-libs 

APP_IMG_LIB_PATH=$STAGING_DIR/lib

cp -v --preserve=links --no-dereference ../../libs/$LIBS_PROFILE/* $APP_IMG_LIB_PATH/

# The following are assumed to be part of the base system
# clean the blacklist in linuxdeployqt and use only explicit one here
# Info from the AppImage exclude list
excludelist=(
ld-linux.so.2 ld-linux-x86-64.so.2 libanl.so.1 libBrokenLocale.so.1 libcidn.so.1 libcrypt.so.1 libc.so.6 libdl.so.2 libm.so.6 libmvec.so.1 libnsl.so.1 libnss_compat.so.2 libnss_db.so.2 libnss_dns.so.2 libnss_files.so.2 libnss_hesiod.so.2 libnss_nisplus.so.2 libnss_nis.so.2 libpthread.so.0 libresolv.so.2 librt.so.1 libthread_db.so.1 libutil.so.1
libstdc++.so.6 libGL.so.1 libdrm.so.2 libgio-2.0.so.0 libasound.so.2 libgdk_pixbuf-2.0.so.0 libfontconfig.so.1 libselinux.so.1
libcom_err.so.2 libcrypt.so.1 libexpat.so.1 libgcc_s.so.1 libglib-2.0.so.0 libgpg-error.so.0 
# libxcb.so.1 # issues with Ubuntu
# libX11.so.6 # issues with Ubuntu
# libgdk-x11-2.0.so.0 # issues with Ubuntu
# libgssapi_krb5.so.2 # Disputed, seemingly needed by Arch Linux since Kerberos is named differently there
# libgssapi.so.3 # Seemingly needed when running Ubuntu 14.04 binaries on Fedora 23
# libhcrypto.so.4 # not found in opensuse
# libheimbase.so.1 # Seemingly needed when running Ubuntu 14.04 binaries on Fedora 23
# libheimntlm.so.0 # Seemingly needed when running Ubuntu 14.04 binaries on Fedora 23
# libhx509.so.5 # not found in opensuse
libICE.so.6 libidn.so.11 libk5crypto.so.3 libkeyutils.so.1
# libkrb5.so.26 # Disputed, seemingly needed by Arch Linux since Kerberos is named differently there
# libkrb5.so.3 # Disputed, seemingly needed by Arch Linux since Kerberos is named differently there
# libkrb5support.so.0 # Disputed, seemingly needed by Arch Linux since Kerberos is named differently there
libp11-kit.so.0
# libpcre.so.3 # Missing on Fedora 24 and on SLED 12 SP1
# libroken.so.18 # not found on opensuse
# libsasl2.so.2 # Seemingly needed when running Ubuntu 14.04 binaries on Fedora 23
libSM.so.6 libusb-1.0.so.0 libuuid.so.1
# libwind.so.0 not found on opensuse
libz.so.1
libgobject-2.0.so.0 libgpg-error.so.0
libdbus-1.so.3)

echo "Excluding: ${excludelist[@]}"

for el in "${excludelist[@]}"
do
    rm -v -f $APP_IMG_LIB_PATH/$el || true
done

# Delete potentially dangerous libraries
rm -f $APP_IMG_LIB_PATH/libstdc* $APP_IMG_LIB_PATH/libgobject* $APP_IMG_LIB_PATH/libc.so.* || true

# Fix the "libGL error" messages
rm -f $APP_IMG_LIB_PATH/libGL.so.* || true
rm -f $APP_IMG_LIB_PATH/libdrm.so.* || true

# remove static lib
rm -f $APP_IMG_LIB_PATH/libhunspell.a

echo -e "\n\n------------------------------"
echo "Generating AppImage"
echo -e "------------------------------\n\n"

rm -v *.AppImage

appimagetool --verbose -n $STAGING_DIR "$APP_NAME-v$VERSION.AppImage"
#LD_LIBRARY_PATH=../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -appimage

popd
# build-Release

echo "Done"
