#include "ui-tests-stubs_plugin.h"
#include <qqml.h>
#include <QQuickImageProvider>

void Ui_Tests_StubsPlugin::registerTypes(const char *uri) {
    Q_UNUSED(uri);
}

void Ui_Tests_StubsPlugin::initializeEngine(QQmlEngine *engine, const char *uri) {
    Q_UNUSED(uri);
    engine->addImageProvider("cached", new QQuickImageProvider(QQmlImageProviderBase::Image));
    engine->addImageProvider("global", new QQuickImageProvider(QQmlImageProviderBase::Image));
}
