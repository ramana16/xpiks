#-------------------------------------------------
#
# Project created by QtCreator 2017-08-26T18:24:58
#
#-------------------------------------------------

QT += gui qml

TARGET = xpks
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

DEFINES += QT_MESSAGELOGCONTEXT

CONFIG(debug, debug|release)  {
    LIBS += -L"$$PWD/../../libs/debug"
} else {
    LIBS += -L"$$PWD/../../libs/release"
}

CONFIG(debug, debug|release)  {
    message("Building debug")
    DEFINES += WITH_PLUGINS
    DEFINES += FAKE_WARNINGS
    #QMAKE_CXXFLAGS += -fsanitize=thread
} else {
    message("Building release")
    DEFINES += WITH_LOGS
}

INCLUDEPATH += ../xpiks-qt

LIBS += -lcurl

macx {
    INCLUDEPATH += "../../vendors/libcurl/include"
}

win32 {
    DEFINES += QT_NO_PROCESS_COMBINED_ARGUMENT_START
    INCLUDEPATH += "../../vendors/libcurl/include"

    LIBS -= -lcurl

    CONFIG(debug, debug|release) {
    EXE_DIR = debug
        LIBS += -llibcurl_debug
        LIBS -= -lquazip
        LIBS += -lquazipd
    }

    CONFIG(release, debug|release) {
        EXE_DIR = release
        LIBS += -llibcurl
    }
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    MetadataIO/metadatareadingworker.h \
    MetadataIO/metadatawritingworker.h \
    MetadataIO/readingorchestrator.h \
    MetadataIO/writingorchestrator.h \
    Connectivity/ftpcoordinator.h \
    Connectivity/conectivityhelpers.h \
    Connectivity/curlftpuploader.h \
    Connectivity/ftpuploaderworker.h \
    Connectivity/uploadbatch.h \
    Connectivity/uploadcontext.h

SOURCES += \
    MetadataIO/metadatareadingworker.cpp \
    MetadataIO/metadatawritingworker.cpp \
    MetadataIO/readingorchestrator.cpp \
    MetadataIO/writingorchestrator.cpp \
    Connectivity/conectivityhelpers.cpp \
    Connectivity/curlftpuploader.cpp \
    Connectivity/ftpcoordinator.cpp \
    Connectivity/ftpuploaderworker.cpp
