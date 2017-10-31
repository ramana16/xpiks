#ifndef FAILEDUPLOADSTEST_H
#define FAILEDUPLOADSTEST_H

#include "integrationtestbase.h"

class FailedUploadsTest : public IntegrationTestBase
{
public:
    FailedUploadsTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // FAILEDUPLOADSTEST_H
