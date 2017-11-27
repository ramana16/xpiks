#ifndef REIMPORTTEST_H
#define REIMPORTTEST_H

#include "integrationtestbase.h"

class ReimportTest: public IntegrationTestBase
{
public:
    ReimportTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // REIMPORTTEST_H
