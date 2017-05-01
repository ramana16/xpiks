#include <iostream>
#include <QDebug>
#include <QCoreApplication>

#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/QMLExtensions/imagecachingservice.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/AutoComplete/autocompleteservice.h"
#include "../../xpiks-qt/Models/deletekeywordsviewmodel.h"
#include "../../xpiks-qt/Conectivity/analyticsuserevent.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "../../xpiks-qt/AutoComplete/autocompletemodel.h"
#include "../../xpiks-qt/Translation/translationmanager.h"
#include "../../xpiks-qt/Translation/translationservice.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"
#include "../../xpiks-qt/Models/recentfilesmodel.h"
#include "../../xpiks-qt/MetadataIO/backupsaverservice.h"
#include "../../xpiks-qt/Suggestion/keywordssuggestor.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Conectivity/telemetryservice.h"
#include "../../xpiks-qt/Helpers/globalimageprovider.h"
#include "../../xpiks-qt/Models/uploadinforepository.h"
#include "../../xpiks-qt/Conectivity/ftpcoordinator.h"
#include "../../xpiks-qt/Models/findandreplacemodel.h"
#include "../../xpiks-qt/Conectivity/curlinithelper.h"
#include "../../xpiks-qt/MetadataIO/exiv2inithelper.h"
#include "../../xpiks-qt/Helpers/helpersqmlwrapper.h"
#include "../../xpiks-qt/Conectivity/updateservice.h"
#include "../../xpiks-qt/Encryption/secretsmanager.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/QMLExtensions/colorsmodel.h"
#include "../../xpiks-qt/Warnings/warningsservice.h"
#include "../../xpiks-qt/Models/artworkproxymodel.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"
#include "../../xpiks-qt/Helpers/clipboardhelper.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Suggestion/locallibrary.h"
#include "../../xpiks-qt/QuickBuffer/quickbuffer.h"
#include "../../xpiks-qt/Models/artworkuploader.h"
#include "../../xpiks-qt/Warnings/warningsmodel.h"
#include "../../xpiks-qt/Plugins/pluginmanager.h"
#include "../../xpiks-qt/Helpers/loggingworker.h"
#include "../../xpiks-qt/Models/languagesmodel.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/ziparchiver.h"
#include "../../xpiks-qt/Helpers/constants.h"
#include "../../xpiks-qt/Helpers/runguard.h"
#include "../../xpiks-qt/Models/logsmodel.h"
#include "../../xpiks-qt/Helpers/logger.h"
#include "../../xpiks-qt/Common/version.h"
#include "../../xpiks-qt/Common/defines.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodelconfig.h"
#include "../../xpiks-qt/Maintenance/maintenanceservice.h"

#include "integrationtestbase.h"
#include "addfilesbasictest.h"
#include "autoattachvectorstest.h"
#include "savefilebasictest.h"
#include "spellcheckmultireplacetest.h"
#include "spellcheckcombinedmodeltest.h"
#include "zipartworkstest.h"
#include "spellcheckundotest.h"
#include "autocompletebasictest.h"
#include "spellingproduceswarningstest.h"
#include "undoaddwithvectorstest.h"
#include "readlegacysavedtest.h"
#include "clearmetadatatest.h"
#include "savewithemptytitletest.h"
#include "jsonmerge_tests.h"
#include "combinededitfixspellingtest.h"
#include "findandreplacemodeltest.h"
#include "addtouserdictionarytest.h"
#include "autodetachvectortest.h"
#include "removefromuserdictionarytest.h"
#include "artworkuploaderbasictest.h"
#include "plaintextedittest.h"
#include "fixspellingmarksmodifiedtest.h"
#include "presetstest.h"
#include "translatorbasictest.h"
#include "userdictedittest.h"
#include "weirdnamesreadtest.h"

#ifdef Q_OS_WIN
#include <tchar.h>
#include <stdio.h>
#include "StackWalker.h"

// minidump
#include <windows.h>
//#include <tchar.h>
#include <dbghelp.h>
//#include <stdio.h>
#include <crtdbg.h>

#if _MSC_VER < 1400
#define strcpy_s(dst, len, src) strcpy(dst, src)
#define strncpy_s(dst, len, src, maxLen) strncpy(dst, len, src)
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

// Specialized stackwalker-output classes
// Console (printf):
class StackWalkerToConsole : public StackWalker
{
protected:
  virtual void OnOutput(LPCSTR szText)
  {
    printf("%s", szText);
  }
};

#if defined _M_X64 || defined _M_IX86
static BOOL PreventSetUnhandledExceptionFilter()
{
  HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
  if (hKernel32 == NULL) return FALSE;
  void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
  if (pOrgEntry == NULL) return FALSE;

#ifdef _M_IX86
  // Code for x86:
  // 33 C0                xor         eax,eax
  // C2 04 00             ret         4
  unsigned char szExecute[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };
#elif _M_X64
  // 33 C0                xor         eax,eax
  // C3                   ret
  unsigned char szExecute[] = { 0x33, 0xC0, 0xC3 };
#else
#error "The following code only works for x86 and x64!"
#endif

  DWORD dwOldProtect = 0;
  BOOL bProt = VirtualProtect(pOrgEntry, sizeof(szExecute),
    PAGE_EXECUTE_READWRITE, &dwOldProtect);

  SIZE_T bytesWritten = 0;
  BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
    pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);

  if ( (bProt != FALSE) && (dwOldProtect != PAGE_EXECUTE_READWRITE))
  {
    DWORD dwBuf;
    VirtualProtect(pOrgEntry, sizeof(szExecute), dwOldProtect, &dwBuf);
  }
  return bRet;
}
#endif


///////////////////////////////////////////////////////////////////////////////
// This function determines whether we need data sections of the given module
//

bool IsDataSectionNeeded( const WCHAR* pModuleName ) {
    // Check parameters

    if( pModuleName == 0 )
    {
        _ASSERTE( _T("Parameter is null.") );
        return false;
    }

    // Extract the module name

    WCHAR szFileName[_MAX_FNAME] = L"";

    _wsplitpath( pModuleName, NULL, NULL, szFileName, NULL );


    // Compare the name with the list of known names and decide

    // Note: For this to work, the executable name must be "mididump.exe"
    if( wcsicmp( szFileName, L"xpiks-qt" ) == 0 )
    {
        return true;
    }
    else if( wcsicmp( szFileName, L"ntdll" ) == 0 )
    {
        return true;
    }

    // Complete

    return false;

}



BOOL CALLBACK MyMiniDumpCallback(
    PVOID                            pParam,
    const PMINIDUMP_CALLBACK_INPUT   pInput,
    PMINIDUMP_CALLBACK_OUTPUT        pOutput
)
{
    BOOL bRet = FALSE;


    // Check parameters

    if( pInput == 0 )
        return FALSE;

    if( pOutput == 0 )
        return FALSE;


    // Process the callbacks

    switch( pInput->CallbackType )
    {
        case IncludeModuleCallback:
        {
            // Include the module into the dump
            bRet = TRUE;
        }
        break;

        case IncludeThreadCallback:
        {
            // Include the thread into the dump
            bRet = TRUE;
        }
        break;

        case ModuleCallback:
        {
            // Are data sections available for this module ?

            if( pOutput->ModuleWriteFlags & ModuleWriteDataSeg )
            {
                // Yes, they are, but do we need them?

                if( !IsDataSectionNeeded( pInput->Module.FullPath ) )
                {
                    wprintf( L"Excluding module data sections: %s \n", pInput->Module.FullPath );

                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
                }
            }

            bRet = TRUE;
        }
        break;

        case ThreadCallback:
        {
            // Include all thread information into the minidump
            bRet = TRUE;
        }
        break;

        case ThreadExCallback:
        {
            // Include this information
            bRet = TRUE;
        }
        break;

        case MemoryCallback:
        {
            // We do not include any information here -> return FALSE
            bRet = FALSE;
        }
        break;

        case CancelCallback:
            break;
    }

    return bRet;

}

void CreateMiniDump( EXCEPTION_POINTERS* pep )
{
    // Open the file

    HANDLE hFile = CreateFile( _T("xpiks-qt.dmp"), GENERIC_READ | GENERIC_WRITE,
                               0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) )
    {
        // Create the minidump

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId           = GetCurrentThreadId();
        mdei.ExceptionPointers  = pep;
        mdei.ClientPointers     = FALSE;

        MINIDUMP_CALLBACK_INFORMATION mci;

        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
        mci.CallbackParam       = 0;

        MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory |
                                                  MiniDumpWithDataSegs |
                                                  MiniDumpWithHandleData |
                                                  MiniDumpWithFullMemoryInfo |
                                                  MiniDumpWithThreadInfo |
                                                  MiniDumpWithUnloadedModules );

        BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
                                     hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci );

        if( !rv )
            _tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() );
        else
            _tprintf( _T("Minidump created.\n") );

        // Close the file

        CloseHandle( hFile );

    }
    else
    {
        _tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() );
    }
}

static BOOL s_bUnhandledExeptionFilterSet = FALSE;
static LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
#ifdef _M_IX86
  if (pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
  {
    static char MyStack[1024*128];  // be sure that we have enought space...
    // it assumes that DS and SS are the same!!! (this is the case for Win32)
    // change the stack only if the selectors are the same (this is the case for Win32)
    //__asm push offset MyStack[1024*128];
    //__asm pop esp;
  __asm mov eax,offset MyStack[1024*128];
  __asm mov esp,eax;
  }
#endif

  {
      StackWalkerToConsole sw;  // output to console
      sw.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);
      fflush(stdout);
  }

  {
      CreateMiniDump(pExPtrs);
  }

  return EXCEPTION_EXECUTE_HANDLER;
}

static void InitUnhandledExceptionFilter()
{
  if (s_bUnhandledExeptionFilterSet == FALSE)
  {
    // set global exception handler (for handling all unhandled exceptions)
    SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
#if defined _M_X64 || defined _M_IX86
    PreventSetUnhandledExceptionFilter();
#endif
    s_bUnhandledExeptionFilterSet = TRUE;
  }
}

#endif // Q_OS_WIN

#if defined(WITH_LOGS)
#undef WITH_LOGS
#endif

#if defined(WITH_PLUGINS)
#undef WITH_PLUGINS
#endif

int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN
    InitUnhandledExceptionFilter();
#endif

    std::cout << "Started integration tests" << std::endl;
    std::cout << "Current working directory:" << QDir::currentPath().toStdString() << std::endl;

    // will call curl_global_init and cleanup
    Conectivity::CurlInitHelper curlInitHelper;
    Q_UNUSED(curlInitHelper);

    MetadataIO::Exiv2InitHelper exiv2InitHelper;
    Q_UNUSED(exiv2InitHelper);

    QCoreApplication app(argc, argv);

    std::cout << "Initialized application" << std::endl;

    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");
    qRegisterMetaType<Common::SpellCheckFlags>("Common::SpellCheckFlags");
    qRegisterMetaTypeStreamOperators<Suggestion::LocalArtworkData>("LocalArtworkData");

    Models::SettingsModel settingsModel;
    settingsModel.initializeConfigs();
    settingsModel.retrieveAllValues();

    Suggestion::LocalLibrary localLibrary;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    if (!appDataPath.isEmpty()) {
        QDir appDataDir(appDataPath);

        QString libraryFilePath = appDataDir.filePath(Constants::LIBRARY_FILENAME);
        localLibrary.setLibraryPath(libraryFilePath);
    } else {
        std::cerr << "AppDataPath is empty!" << std::endl;
    }

    Models::LogsModel logsModel;
    logsModel.startLogging();

    QMLExtensions::ColorsModel colorsModel;

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::UploadInfoRepository uploadInfoRepository;
    KeywordsPresets::PresetKeywordsModel presetsModel;
    KeywordsPresets::PresetKeywordsModelConfig presetsModelConfig;
    Warnings::WarningsService warningsService;
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Models::RecentFilesModel recentFileModel;
    Conectivity::FtpCoordinator *ftpCoordinator = new Conectivity::FtpCoordinator(settingsModel.getMaxParallelUploads());
    Models::ArtworkUploader artworkUploader(ftpCoordinator);
    SpellCheck::SpellCheckerService spellCheckerService(&settingsModel);
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;
    MetadataIO::BackupSaverService metadataSaverService;
    Warnings::WarningsModel warningsModel;
    warningsModel.setSourceModel(&artItemsModel);
    Models::LanguagesModel languagesModel;
    AutoComplete::AutoCompleteModel autoCompleteModel;
    AutoComplete::AutoCompleteService autoCompleteService(&autoCompleteModel);
    QMLExtensions::ImageCachingService imageCachingService;
    Models::FindAndReplaceModel findAndReplaceModel(&colorsModel);
    Models::DeleteKeywordsViewModel deleteKeywordsModel;
    Translation::TranslationManager translationManager;
    Translation::TranslationService translationService(translationManager);
    Models::ArtworkProxyModel artworkProxy;
    // intentional memory leak to beat spellcheck lock stuff
    QuickBuffer::QuickBuffer quickBuffer;
    Maintenance::MaintenanceService maintenanceService;

    Conectivity::UpdateService updateService(&settingsModel);

    MetadataIO::MetadataIOCoordinator metadataIOCoordinator;
    Conectivity::TelemetryService telemetryService("1234567890", false);
    Plugins::PluginManager pluginManager;

    Commands::CommandManager commandManager;
    commandManager.InjectDependency(&artworkRepository);
    commandManager.InjectDependency(&artItemsModel);
    commandManager.InjectDependency(&filteredArtItemsModel);
    commandManager.InjectDependency(&combinedArtworksModel);
    commandManager.InjectDependency(&artworkUploader);
    commandManager.InjectDependency(&uploadInfoRepository);
    commandManager.InjectDependency(&warningsService);
    commandManager.InjectDependency(&secretsManager);
    commandManager.InjectDependency(&undoRedoManager);
    commandManager.InjectDependency(&zipArchiver);
    commandManager.InjectDependency(&keywordsSuggestor);
    commandManager.InjectDependency(&settingsModel);
    commandManager.InjectDependency(&recentDirectorieModel);
    commandManager.InjectDependency(&recentFileModel);
    commandManager.InjectDependency(&spellCheckerService);
    commandManager.InjectDependency(&spellCheckSuggestionModel);
    commandManager.InjectDependency(&metadataSaverService);
    commandManager.InjectDependency(&telemetryService);
    commandManager.InjectDependency(&updateService);
    commandManager.InjectDependency(&logsModel);
    commandManager.InjectDependency(&localLibrary);
    commandManager.InjectDependency(&metadataIOCoordinator);
    commandManager.InjectDependency(&pluginManager);
    commandManager.InjectDependency(&languagesModel);
    commandManager.InjectDependency(&colorsModel);
    commandManager.InjectDependency(&autoCompleteService);
    commandManager.InjectDependency(&imageCachingService);
    commandManager.InjectDependency(&findAndReplaceModel);
    commandManager.InjectDependency(&deleteKeywordsModel);
    commandManager.InjectDependency(&presetsModel);
    commandManager.InjectDependency(&presetsModelConfig);
    commandManager.InjectDependency(&translationManager);
    commandManager.InjectDependency(&translationService);
    commandManager.InjectDependency(&artworkProxy);
    commandManager.InjectDependency(&quickBuffer);
    commandManager.InjectDependency(&maintenanceService);

    commandManager.ensureDependenciesInjected();

    keywordsSuggestor.initSuggestionEngines();

    secretsManager.setMasterPasswordHash(settingsModel.getMasterPasswordHash());
    uploadInfoRepository.initFromString(settingsModel.getUploadHosts());
    recentDirectorieModel.deserializeFromSettings(settingsModel.getRecentDirectories());
    recentFileModel.deserializeFromSettings(settingsModel.getRecentFiles());

#if defined(APPVEYOR)
    settingsModel.setExifToolPath("c:/projects/xpiks-deps/windows-3rd-party-bin/exiftool.exe");
#endif

    commandManager.connectEntitiesSignalsSlots();    
    commandManager.afterConstructionCallback();

    int result = 0;

    QVector<IntegrationTestBase*> integrationTests;

    integrationTests.append(new AddFilesBasicTest(&commandManager));
    integrationTests.append(new AutoAttachVectorsTest(&commandManager));
    integrationTests.append(new SaveFileBasicTest(&commandManager));
    integrationTests.append(new SpellCheckMultireplaceTest(&commandManager));
    integrationTests.append(new SpellCheckCombinedModelTest(&commandManager));
    integrationTests.append(new ZipArtworksTest(&commandManager));
    integrationTests.append(new SpellCheckUndoTest(&commandManager));
    integrationTests.append(new AutoCompleteBasicTest(&commandManager));
    integrationTests.append(new SpellingProducesWarningsTest(&commandManager));
    integrationTests.append(new UndoAddWithVectorsTest(&commandManager));
    integrationTests.append(new ReadLegacySavedTest(&commandManager));
    integrationTests.append(new ClearMetadataTest(&commandManager));
    integrationTests.append(new SaveWithEmptyTitleTest(&commandManager));
    integrationTests.append(new JsonMergeTests(&commandManager));
    integrationTests.append(new CombinedEditFixSpellingTest(&commandManager));
    integrationTests.append(new FindAndReplaceModelTest(&commandManager));
    integrationTests.append(new AddToUserDictionaryTest(&commandManager));
    integrationTests.append(new AutoDetachVectorTest(&commandManager));
    integrationTests.append(new RemoveFromUserDictionaryTest(&commandManager));
    integrationTests.append(new ArtworkUploaderBasicTest(&commandManager));
    integrationTests.append(new PlainTextEditTest(&commandManager));
    integrationTests.append(new FixSpellingMarksModifiedTest(&commandManager));
    integrationTests.append(new PresetsTest(&commandManager));
    integrationTests.append(new TranslatorBasicTest(&commandManager));
    integrationTests.append(new UserDictEditTest(&commandManager));
    integrationTests.append(new WeirdNamesReadTest(&commandManager));

    qDebug("\n");
    int succeededTestsCount = 0, failedTestsCount = 0;
    QStringList failedTests;

    foreach (IntegrationTestBase *test, integrationTests) {
        QThread::msleep(500);

        qDebug("---------------------------------------------------------");
        qInfo("Running test: %s", test->testName().toStdString().c_str());

        try {
            test->setup();
            int testResult = test->doTest();
            test->teardown();

            result += testResult;
            if (testResult == 0) {
                succeededTestsCount++;
                qInfo("Test %s PASSED", test->testName().toStdString().c_str());
            } else {
                failedTestsCount++;
                qInfo("Test %s FAILED", test->testName().toStdString().c_str());
                failedTests.append(test->testName());
            }
        }
        catch (...) {
            qInfo("Test %s CRASHED", test->testName().toStdString().c_str());
            result += 1;
        }

        qDebug("\n");
    }

    qDeleteAll(integrationTests);

    commandManager.beforeDestructionCallback();

    qInfo() << "--------------------------";
    qInfo() << "Integration Tests Results:" << succeededTestsCount << "succeeded," << failedTestsCount << "failed";

    if (!failedTests.empty()) {
        qInfo() << "FAILED TESTS:" << failedTests.join(", ");
    }

    qInfo() << "--------------------------";

    // for the logs to appear
    app.processEvents();

    QThread::sleep(1);

    return result;
}

