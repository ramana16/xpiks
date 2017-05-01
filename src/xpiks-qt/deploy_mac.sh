#!/bin/bash

APP_NAME=xpiks-qt
VERSION="1.4.2"
VOL_NAME="Xpiks"

DMG_BACKGROUND_IMG="dmg-background.jpg"
DMG_BACKGROUND_PATH="deps/$DMG_BACKGROUND_IMG"

DMG_TMP="xpiks-qt-v${VERSION}.tmp.dmg"
DMG_FINAL="xpiks-qt-v${VERSION}.dmg"
STAGING_DIR="./osx_deploy"

BUILD_DIR=build-Release

if [ -d "/Volumes/${VOL_NAME}" ]; then
    echo "Please unmount existing Volume ${VOL_NAME} first!"
    exit
fi

pushd "$BUILD_DIR"

~/Qt5.6.2/5.6/clang_64/bin/macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../ -qmldir=../Components/ -qmldir=../Constants/ -qmldir=../Dialogs/ -qmldir=../StyledControls/ -qmldir=../StackViews/ -qmldir=../CollapserTabs/

popd

echo "Copying libraries..."

LIBS_PATH="../../libs"

cp "$LIBS_PATH/libssdll.1.0.0.dylib" "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"
cp "$LIBS_PATH/libface.1.0.0.dylib" "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"
cp "$LIBS_PATH/libquazip.1.0.0.dylib" "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"

echo "Changing dependency path..."

install_name_tool -change libssdll.1.dylib @executable_path/../Frameworks/libssdll.1.dylib "$BUILD_DIR/xpiks-qt.app/Contents/MacOS/xpiks-qt"
install_name_tool -change libface.1.dylib @executable_path/../Frameworks/libface.1.dylib "$BUILD_DIR/xpiks-qt.app/Contents/MacOS/xpiks-qt"
install_name_tool -change libquazip.1.dylib @executable_path/../Frameworks/libquazip.1.dylib "$BUILD_DIR/xpiks-qt.app/Contents/MacOS/xpiks-qt"

echo "Linking dynamic libraries.."

pushd "$BUILD_DIR/xpiks-qt.app/Contents/Frameworks/"
ln -s libssdll.1.0.0.dylib libssdll.1.dylib
ln -s libquazip.1.0.0.dylib libquazip.1.dylib
ln -s libface.1.0.0.dylib libface.1.dylib
popd

# ------------------------------

# clear out any old data
rm -rf "${STAGING_DIR}" "${DMG_TMP}" "${DMG_FINAL}"
# copy over the stuff we want in the final disk image to our staging dir
mkdir -p "${STAGING_DIR}"
cp -rpf "$BUILD_DIR/${APP_NAME}.app" "${STAGING_DIR}"

echo "App dir is packed. Creating DMG..."

# figure out how big our DMG needs to be
#  assumes our contents are at least 1M!
SIZE=`du -sh "${STAGING_DIR}" | sed 's/\([0-9\.]*\)M\(.*\)/\1/'`
SIZE=`echo "${SIZE} + 32.0" | bc | awk '{print int($1+0.5)}'`

if [ $? -ne 0 ]; then
   echo "Error: Cannot compute size of staging dir"
   exit
fi

hdiutil create -srcfolder "${STAGING_DIR}" -volname "${VOL_NAME}" -fs HFS+ \
-fsargs "-c c=64,a=16,e=16" -format UDRW -size ${SIZE}m "${DMG_TMP}"

echo "Created DMG: ${DMG_TMP}. Mounting..."

# mount it and save the device
DEVICE=$(hdiutil attach -readwrite -noverify "${DMG_TMP}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

sleep 2

# add a link to the Applications dir
echo "Add link to /Applications"
pushd /Volumes/"${VOL_NAME}"
ln -s /Applications
popd


# add a background image
mkdir /Volumes/"${VOL_NAME}"/.background
cp "${DMG_BACKGROUND_PATH}" /Volumes/"${VOL_NAME}"/.background/

# tell the Finder to resize the window, set the background,
#  change the icon size, place the icons in the right position, etc.
echo '
   tell application "Finder"
     tell disk "'${VOL_NAME}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {480, 300, 980, 570}
           set statusbar visible of container window to false
           set position of every item to {600, 400}
           set viewOptions to the icon view options of container window
           set arrangement of viewOptions to not arranged
           set icon size of viewOptions to 128
           set background picture of viewOptions to file ".background:'${DMG_BACKGROUND_IMG}'"
           set position of item "'${APP_NAME}'.app" of container window to {110, 135}
           set position of item "Applications" of container window to {395, 130}
           set name of item "Applications" to " "
           close
           open
           update without registering applications
           delay 2
     end tell
   end tell
' | osascript

sync

# unmount it
hdiutil detach "${DEVICE}"

# now make the final image a compressed disk image
echo "Creating compressed image"
hdiutil convert "${DMG_TMP}" -format UDZO -imagekey zlib-level=9 -o "${DMG_FINAL}"

shasum "${DMG_FINAL}"

# clean up
rm -rf "${DMG_TMP}"
rm -rf "${STAGING_DIR}"

echo 'Done.'
