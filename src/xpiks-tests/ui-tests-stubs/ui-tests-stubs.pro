TEMPLATE = lib
TARGET = uitestsstubs
QT += qml quick
CONFIG += qt plugin c++11

DESTDIR = "$$PWD/../xpiks-tests-ui/UiTestsStubPlugin"

# Input
SOURCES += \
    ui-tests-stubs_plugin.cpp

HEADERS += \
    ui-tests-stubs_plugin.h

DISTFILES = qmldir

win32 {
    copyqmldir.commands = $(COPY_FILE) \"$$shell_path($$PWD/qmldir)\" \"$$shell_path($$DESTDIR/)\"
} else {
    copyqmldir.commands = $(COPY_FILE) "$$PWD/qmldir" "$$DESTDIR/"
}

QMAKE_EXTRA_TARGETS += copyqmldir
POST_TARGETDEPS += copyqmldir
