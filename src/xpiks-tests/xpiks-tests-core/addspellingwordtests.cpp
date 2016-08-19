#include "addspellingwordtests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/SpellCheck/spellcheckiteminfo.h"
#include "Mocks/spellcheckservicemock.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include <QObject>

#define INIT_ADD_SPELLING_TEST(count) \
    Mocks::CommandManagerMock commandManager; \
    Mocks::SpellCheckServiceMock spellCheckService; \
    commandManager.InjectDependency(&spellCheckService); \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    Models::ArtworksRepository artworksRepository; \
    commandManager.InjectDependency(&artworksRepository); \
    commandManager.InjectDependency(&artItemsModelMock); \
    commandManager.generateAndAddArtworks(count); \
    QObject::connect(&spellCheckService, SIGNAL(uDictStateChanged(QString)), &artItemsModelMock, SIGNAL(uDictStateChanged(QString)));

