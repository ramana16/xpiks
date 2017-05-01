echo 'Starting deployment...'

set XPIKS_PLATFORM=x64
set XPIKS_VERSION=1.4.2

set XPIKS_DEPS_PATH=%cd%\..\..\..\xpiks-deps
set WINDOWS_LIBS=%XPIKS_DEPS_PATH%\windows-libs\release-%XPIKS_PLATFORM%
set REDIST_LIBS=%XPIKS_DEPS_PATH%\windows-libs\redist-%XPIKS_PLATFORM%
set OPENSSL_LIBS=%XPIKS_DEPS_PATH%\windows-libs\openssl-%XPIKS_PLATFORM%

pushd ..\build-xpiks-qt-Desktop_Qt_*-Release\release

windeployqt.exe --release --verbose=2 --qmldir=../../xpiks-qt/CollapserTabs/ --qmldir=../../xpiks-qt/Components/ --qmldir=../../xpiks-qt/Constants/ --qmldir=../../xpiks-qt/Dialogs/ --qmldir=../../xpiks-qt/StackViews/ --qmldir=../../xpiks-qt/StyledControls/ --qmldir=../../xpiks-qt/ xpiks-qt.exe

echo 'Copying librries...'

xcopy /Y /s %WINDOWS_LIBS% .
xcopy /Y /s %REDIST_LIBS% .

copy /Y %OPENSSL_LIBS%\*.dll .

copy /Y ..\..\..\libs\libexiv2.dll .
copy /Y ..\..\..\libs\libcurl*.dll .
copy /Y ..\..\..\libs\quazip*.dll .
copy /Y ..\..\..\libs\ssdll.dll .
copy /Y ..\..\..\libs\mman.dll .
copy /Y ..\..\..\libs\face.dll .
copy /Y ..\..\..\libs\libexpat.dll .
copy /Y ..\..\..\libs\z.dll .

copy /Y ..\..\..\..\ministaller\src\ministaller.exe .

del *.obj
del *.cpp
del *.lib
del *.pdb
del vcredist_%XPIKS_PLATFORM%.exe

cd ..

echo 'Packing binaries...'

ren release Xpiks
"C:\Program Files\7-Zip\7z" a xpiks-qt-v%XPIKS_VERSION%.zip Xpiks

ren Xpiks xpiks-qt-v%XPIKS_VERSION%-tmp

popd

echo 'Done'
