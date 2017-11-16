#ifndef MASTERPASSWORDTEST_H
#define MASTERPASSWORDTEST_H

#include "integrationtestbase.h"

class MasterPasswordTest: public IntegrationTestBase
{
public:
    MasterPasswordTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    { }

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // MASTERPASSWORDTEST_H
