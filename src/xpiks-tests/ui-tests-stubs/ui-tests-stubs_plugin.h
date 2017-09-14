#ifndef UI_TESTS_STUBS_PLUGIN_H
#define UI_TESTS_STUBS_PLUGIN_H

#include <QQmlExtensionPlugin>

class Ui_Tests_StubsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    virtual void registerTypes(const char *uri) override;
    virtual void initializeEngine(QQmlEngine *engine, const char *uri) override;
};

#endif // UI_TESTS_STUBS_PLUGIN_H
