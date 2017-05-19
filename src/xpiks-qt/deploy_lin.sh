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

DEPLOY_COMMAND="$DEPLOY_TOOL $STAGING_DIR/$APP_NAME -verbose=3 -no-strip -qmldir=$SRC_ROOT -qmldir=$SRC_ROOT/Components/ -qmldir=$SRC_ROOT/Constants/ -qmldir=$SRC_ROOT/Dialogs/ -qmldir=$SRC_ROOT/StyledControls/ -qmldir=$SRC_ROOT/StackViews/ -qmldir=$SRC_ROOT/CollapserTabs/"

LD_LIBRARY_PATH=../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -bundle-non-qt-libs 

APP_IMG_LIB_PATH=$STAGING_DIR/lib

cp -v --preserve=links --no-dereference ../../libs/$LIBS_PROFILE/* $APP_IMG_LIB_PATH/

# The following are assumed to be part of the base system

rm -f $APP_IMG_LIB_PATH/libcom_err.so.2 || true
rm -f $APP_IMG_LIB_PATH/libcrypt.so.1 || true
rm -f $APP_IMG_LIB_PATH/libdl.so.2 || true
# rm -f $APP_IMG_LIB_PATH/libexpat.so.1 || true # needed for xpiks
rm -f $APP_IMG_LIB_PATH/libfontconfig.so.1 || true
rm -f $APP_IMG_LIB_PATH/libgcc_s.so.1 || true
rm -f $APP_IMG_LIB_PATH/libglib-2.0.so.0 || true
rm -f $APP_IMG_LIB_PATH/libgpg-error.so.0 || true
rm -f $APP_IMG_LIB_PATH/libgssapi_krb5.so.2 || true
rm -f $APP_IMG_LIB_PATH/libgssapi.so.3 || true
rm -f $APP_IMG_LIB_PATH/libhcrypto.so.4 || true
rm -f $APP_IMG_LIB_PATH/libheimbase.so.1 || true
rm -f $APP_IMG_LIB_PATH/libheimntlm.so.0 || true
rm -f $APP_IMG_LIB_PATH/libhx509.so.5 || true
rm -f $APP_IMG_LIB_PATH/libICE.so.6 || true
rm -f $APP_IMG_LIB_PATH/libidn.so.11 || true
rm -f $APP_IMG_LIB_PATH/libk5crypto.so.3 || true
rm -f $APP_IMG_LIB_PATH/libkeyutils.so.1 || true
rm -f $APP_IMG_LIB_PATH/libkrb5.so.26 || true
rm -f $APP_IMG_LIB_PATH/libkrb5.so.3 || true
rm -f $APP_IMG_LIB_PATH/libkrb5support.so.0 || true
# rm -f $APP_IMG_LIB_PATH/liblber-2.4.so.2 || true # needed for debian wheezy
# rm -f $APP_IMG_LIB_PATH/libldap_r-2.4.so.2 || true # needed for debian wheezy
rm -f $APP_IMG_LIB_PATH/libm.so.6 || true
rm -f $APP_IMG_LIB_PATH/libp11-kit.so.0 || true
rm -f $APP_IMG_LIB_PATH/libpcre.so.3 || true
rm -f $APP_IMG_LIB_PATH/libpthread.so.0 || true
rm -f $APP_IMG_LIB_PATH/libresolv.so.2 || true
rm -f $APP_IMG_LIB_PATH/libroken.so.18 || true
rm -f $APP_IMG_LIB_PATH/librt.so.1 || true
rm -f $APP_IMG_LIB_PATH/libsasl2.so.2 || true
rm -f $APP_IMG_LIB_PATH/libSM.so.6 || true
rm -f $APP_IMG_LIB_PATH/libusb-1.0.so.0 || true
rm -f $APP_IMG_LIB_PATH/libuuid.so.1 || true
rm -f $APP_IMG_LIB_PATH/libwind.so.0 || true
rm -f $APP_IMG_LIB_PATH/libz.so.1 || true

# Delete potentially dangerous libraries
rm -f $APP_IMG_LIB_PATH/libstdc* $APP_IMG_LIB_PATH/libgobject* $APP_IMG_LIB_PATH/libc.so.* || true

rm -f $APP_IMG_LIB_PATH/libdbus-1.so.3 || true

# Fix the "libGL error" messages
rm -f $APP_IMG_LIB_PATH/libGL.so.* || true
rm -f $APP_IMG_LIB_PATH/libdrm.so.* || true

echo -e "\n\n------------------------------"
echo "Generating AppImage"
echo -e "------------------------------\n\n"

rm -v *.AppImage

LD_LIBRARY_PATH=../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -appimage

popd
# build-Release

echo "Done"
