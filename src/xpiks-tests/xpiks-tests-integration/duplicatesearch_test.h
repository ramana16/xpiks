#ifndef DUPLICATESEARCH_TEST_H
#define DUPLICATESEARCH_TEST_H

#include "integrationtestbase.h"

class DuplicateSearchTest: public IntegrationTestBase
{
public:
    DuplicateSearchTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // DUPLICATESEARCH_TEST_H
