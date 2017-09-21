#ifndef AUTOCOMPLETEPRESETSTEST_H
#define AUTOCOMPLETEPRESETSTEST_H

#include "integrationtestbase.h"

class AutoCompletePresetsTest : public IntegrationTestBase
{
public:
    AutoCompletePresetsTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName() override;
    virtual void setup() override;
    virtual void teardown() override;
    virtual int doTest() override;
};

#endif // AUTOCOMPLETEPRESETSTEST_H
