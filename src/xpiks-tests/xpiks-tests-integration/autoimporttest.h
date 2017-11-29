#ifndef AUTOIMPORTTEST_H
#define AUTOIMPORTTEST_H

#include "integrationtestbase.h"

class AutoImportTest : public IntegrationTestBase
{
public:
    AutoImportTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // AUTOIMPORTTEST_H
