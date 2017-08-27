#ifndef SAVEDIRECTEXPORTTEST_H
#define SAVEDIRECTEXPORTTEST_H

#include "integrationtestbase.h"

class SaveFileLegacyTest : public IntegrationTestBase
{
public:
    SaveFileLegacyTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SAVEDIRECTEXPORTTEST_H
