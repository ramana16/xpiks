#!/bin/bash

cd vendors/

##########

tar xzf /tmp/hunspell.tar.gz
cd hunspell/
qmake "CONFIG+=debug" hunspell.pro
make
pwd
ls -la
cp libhunspell.a ../../libs/libhunspell.a
cd ../

##########

cd quazip/quazip/
qmake "CONFIG+=debug" quazip.pro
make
cp libquazip.so.1.0.0 ../../../libs/
cd ../../../libs
ln -s libquazip.so.1.0.0 libquazip.so
ln -s libquazip.so.1.0.0 libquazip.so.1
ln -s libquazip.so.1.0.0 libquazip.so.1.0

##########

cd ../vendors/
mv tiny-aes/aes.c tiny-aes/aes.cpp

##########

cd cpp-libface
make lib-face
cp libface.a ../../libs/
cd ../

##########

cd ssdll/src/ssdll
qmake "CONFIG+=debug" ssdll.pro
make
cp libssdll.so.1.0.0 ../../../../libs/
cd ../../../../libs
ln -s libssdll.so.1.0.0 libssdll.so
ln -s libssdll.so.1.0.0 libssdll.so.1
ln -s libssdll.so.1.0.0 libssdll.so.1.0
cd ../src

##########

cd libxpks_stub
qmake "CONFIG+=debug" libxpks_stub.pro
make
cp libxpks.a ../../libs/

##########