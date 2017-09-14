TEMPLATE = app

QT += qml quick
CONFIG += qmltestcase plugin c++11
TARGET = xpiks-tests-ui

DEFINES += QML_IMPORT_TRACE
DEFINES += QT_DEBUG_PLUGINS

SOURCES += main.cpp \
    ../../xpiks-qt/QMLExtensions/triangleelement.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = "$$PWD/UiTestsStubPlugin"

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    ../../xpiks-qt/Components/EditableTags.qml \
    tst_EditableTags.qml \
    FakeColors.qml \
    ../../xpiks-qt/StackViews/CombinedEditView.qml \
    tst_CombinedEditView.qml

HEADERS += \
    ../../xpiks-qt/Helpers/clipboardhelper.h \
    ../../xpiks-qt/QMLExtensions/triangleelement.h

RESOURCES += \
    xpiks-tests-ui.qrc

STUB_PLUGIN_NAME = UiTestsStubPlugin

win32 {
    CONFIG(debug, debug|release) {
        BUILDDIR = $$OUT_PWD/debug
    }
    else: build_pass {
        BUILDDIR = $$OUT_PWD/release
    }

    mkstubdir.commands = $$QMAKE_MKDIR_CMD \"$$shell_path($$BUILDDIR/$$STUB_PLUGIN_NAME)\"
    QMAKE_EXTRA_TARGETS += mkstubdir
    POST_TARGETDEPS += mkstubdir

    copyplugins.commands = $(COPY_DIR) \"$$shell_path($$PWD/$$STUB_PLUGIN_NAME)\" \"$$shell_path($$BUILDDIR/$$STUB_PLUGIN_NAME)\"
} else {
    copyplugins.commands = $$QMAKE_COPY_DIR "$$PWD/$$STUB_PLUGIN_NAME/" "$$OUT_PWD/"
}

QMAKE_EXTRA_TARGETS += copyplugins
POST_TARGETDEPS += copyplugins

travis-ci {
    QMAKE_EXTRA_TARGETS -= copyplugins
    POST_TARGETDEPS -= copyplugins
}

appveyor {
    QMAKE_EXTRA_TARGETS -= copyplugins
    POST_TARGETDEPS -= copyplugins
}
