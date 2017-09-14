#include "ui-tests-stubs_plugin.h"
#include <qqml.h>
#include <QSize>
#include <QImage>
#include <QQuickImageProvider>

class FakeImageProvider: public QQuickImageProvider
{
public:
    FakeImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    {
    }

public:
    virtual QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override {
        Q_UNUSED(id);
        Q_UNUSED(requestedSize);

        const int width = 300;
        const int height = 300;

        if (size) {
            *size = QSize(width, height);
        }

        QImage img(width, height, QImage::Format_RGB888);
        img.fill(Qt::black);

        return img;
    }
};

void Ui_Tests_StubsPlugin::registerTypes(const char *uri) {
    Q_UNUSED(uri);
}

void Ui_Tests_StubsPlugin::initializeEngine(QQmlEngine *engine, const char *uri) {
    Q_UNUSED(uri);
    engine->addImageProvider("cached", new FakeImageProvider());
    engine->addImageProvider("global", new FakeImageProvider());
}
