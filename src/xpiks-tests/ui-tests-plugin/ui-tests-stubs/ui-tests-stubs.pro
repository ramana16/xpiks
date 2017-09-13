TEMPLATE = lib
TARGET = uitestsstubs
QT += qml quick
CONFIG += qt plugin c++11

DESTDIR = "$$PWD/../../xpiks-tests-ui/UiTestsStubPlugin"

# Input
SOURCES += \
    ui-tests-stubs_plugin.cpp

HEADERS += \
    ui-tests-stubs_plugin.h

DISTFILES = qmldir \
    ui-tests-stubs.qmlproject

copyqmldir.commands = $(COPY_FILE) "$$PWD/qmldir" "$$DESTDIR/"
QMAKE_EXTRA_TARGETS += copyqmldir
POST_TARGETDEPS += copyqmldir

#!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
#    copy_qmldir.target = $$OUT_PWD/qmldir
#    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
#    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
#    QMAKE_EXTRA_TARGETS += copy_qmldir
#    PRE_TARGETDEPS += $$copy_qmldir.target
#}

#qmldir.files = qmldir
#unix {
#    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
#    qmldir.path = $$installPath
#    target.path = $$installPath
#    INSTALLS += target qmldir
#}
