#ifndef METADATACACHESAVETEST_H
#define METADATACACHESAVETEST_H

#include "integrationtestbase.h"

class MetadataCacheSaveTest : public IntegrationTestBase
{
public:
    MetadataCacheSaveTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // METADATACACHESAVETEST_H
