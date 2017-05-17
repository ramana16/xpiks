#!/bin/bash

APP_NAME=xpiks-qt
VERSION="1.4.2"
APPDIR_NAME="Xpiks"

STAGING_DIR="./linux_deploy"

APP_PROFILE=Debug
LIBS_PROFILE=debug
DEPLOY_TOOL=linuxdeployqt

echo "------------------------------"

pushd ../build-xpiks-qt-*${APP_PROFILE}

# clear out any old data
echo "Wiping old data"
rm -v -rf "${STAGING_DIR}"

export PATH=~/bin:~/Qt5.6.2/5.6/gcc_64/bin:$PATH

mkdir -p $STAGING_DIR

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

pushd $STAGING_DIR

echo "Working in directory: $(pwd)"

DEPLOY_COMMAND="$DEPLOY_TOOL $APP_NAME -verbose=3 -bundle-non-qt-libs -no-strip -qmldir=../../ -qmldir=../../Components/ -qmldir=../../Constants/ -qmldir=../../Dialogs/ -qmldir=../../StyledControls/ -qmldir=../../StackViews/ -qmldir=../../CollapserTabs/"

LD_LIBRARY_PATH=../../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND

APP_IMG_LIB_PATH=./lib

cp -v --preserve=links --no-dereference ../../../libs/release/* $APP_IMG_LIB_PATH/

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

LD_LIBRARY_PATH=../../../libs/$LIBS_PROFILE:$LD_LIBRARY_PATH $DEPLOY_COMMAND -appimage

popd
# staging

popd
# build-Release

echo "Done"
