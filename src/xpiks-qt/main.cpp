/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <QDir>
#include <QtQml>
#include <QFile>
#include <QUuid>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QQmlContext>
#include <QApplication>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QQmlApplicationEngine>
#include <QDesktopWidget>
//-------------------------------------
#include "SpellCheck/spellchecksuggestionmodel.h"
#include "Models/filteredartitemsproxymodel.h"
#include "Suggestion/suggestionqueryengine.h"
#include "MetadataIO/metadataiocoordinator.h"
#include "Conectivity/analyticsuserevent.h"
#include "SpellCheck/spellcheckerservice.h"
#include "Models/recentdirectoriesmodel.h"
#include "MetadataIO/backupsaverservice.h"
#include "Suggestion/keywordssuggestor.h"
#include "Models/combinedartworksmodel.h"
#include "Conectivity/telemetryservice.h"
#include "Helpers/globalimageprovider.h"
#include "Models/uploadinforepository.h"
#include "Helpers/helpersqmlwrapper.h"
#include "Encryption/secretsmanager.h"
#include "Models/artworksrepository.h"
#include "Helpers/settingsprovider.h"
#include "Warnings/warningsservice.h"
#include "UndoRedo/undoredomanager.h"
#include "Helpers/clipboardhelper.h"
#include "Commands/commandmanager.h"
#include "Suggestion/locallibrary.h"
#include "Models/artworkuploader.h"
#include "Warnings/warningsmodel.h"
#include "Plugins/pluginmanager.h"
#include "Helpers/loggingworker.h"
#include "Helpers/updateservice.h"
#include "Models/artitemsmodel.h"
#include "Models/settingsmodel.h"
#include "Helpers/appsettings.h"
#include "Models/ziparchiver.h"
#include "Helpers/constants.h"
#include "Helpers/runguard.h"
#include "Models/logsmodel.h"
#include "Helpers/logger.h"
#include "Common/version.h"
#include "Common/defines.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);

    QString logLine;
    QString time = QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy hh:mm:ss.zzz");
    switch (type) {
        case QtDebugMsg:
            logLine = QString("%1 - Debug: %2").arg(time).arg(msg);
            break;
        case QtWarningMsg:
            logLine = QString("%1 - Warning: %2").arg(time).arg(msg);
            break;
        case QtCriticalMsg:
            logLine = QString("%1 - Critical: %2").arg(time).arg(msg);
            break;
        case QtFatalMsg:
            logLine = QString("%1 - Fatal: %2").arg(time).arg(msg);
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 1))
        case QtInfoMsg:
            logLine = QString("%1 - Info:  %2").arg(time).arg(msg);
            break;
#endif
    }

    Helpers::Logger &logger = Helpers::Logger::getInstance();
    logger.log(logLine);

    if (type == QtFatalMsg) {
        abort();
    }
}

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

void initQSettings() {
    QCoreApplication::setOrganizationName(Constants::ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(Constants::ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Constants::APPLICATION_NAME);
    QString appVersion(STRINGIZE(BUILDNUMBER));
    QCoreApplication::setApplicationVersion(STRINGIZE(XPIKS_VERSION)" "STRINGIZE(XPIKS_VERSION_SUFFIX)" - " + appVersion.left(10));
}

void ensureUserIdExists(Helpers::AppSettings *settings) {
    QLatin1String userIdKey = QLatin1String(Constants::USER_AGENT_ID);
    if (!settings->contains(userIdKey)) {
        QUuid uuid = QUuid::createUuid();
        settings->setValue(userIdKey, uuid.toString());
    }
}

static const char *setHighDpiEnvironmentVariable()
{
    const char* envVarName = 0;
    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";

#ifdef Q_OS_WIN
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
    if (isWindows
            && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)) {
        envVarName = ENV_VAR_QT_DEVICE_PIXEL_RATIO;
        qputenv(envVarName, "auto");
    }
#else
    if (isWindows
            && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) // legacy in 5.6, but still functional
            && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
#endif // < Qt 5.6
    return envVarName;
}

int main(int argc, char *argv[]) {
    Helpers::RunGuard guard("xpiks");
    if (!guard.tryToRun()) {
        std::cerr << "Xpiks is already running";
        return -1;
    }

    const char *highDpiEnvironmentVariable = setHighDpiEnvironmentVariable();

    initQSettings();
    Helpers::AppSettings appSettings;
    ensureUserIdExists(&appSettings);

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
        std::cerr << "AppDataPath is empty!";
    }

#ifdef WITH_LOGS
    const QString &logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
    if (!logFileDir.isEmpty()) {
        QDir dir(logFileDir);
        if (!dir.exists()) {
            bool created = QDir().mkpath(logFileDir);
            Q_UNUSED(created);
        }

        QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmmss-zzz");
        QString logFilename = QString("xpiks-qt-%1.log").arg(time);

        QString logFilePath = dir.filePath(logFilename);

        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.setLogFilePath(logFilePath);
    }

#endif

    Models::LogsModel logsModel;
    logsModel.startLogging();

    qInstallMessageHandler(myMessageHandler);
    qInfo() << "main #" << "Log started." << "Xpiks" << XPIKS_VERSION_STRING << "-" << STRINGIZE(BUILDNUMBER);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    qInfo() << "main #" << QSysInfo::productType() << QSysInfo::productVersion() << QSysInfo::currentCpuArchitecture();
#else
#ifdef Q_OS_WIN
    qInfo() << "main #" << QLatin1String("Windows Qt<5.4");
#elsif Q_OS_DARWIN
    qInfo() << "main #" << QLatin1String("OS X Qt<5.4");
#else
    qInfo() << "main #" << QLatin1String("LINUX Qt<5.4");
#endif
#endif

    QApplication app(argc, argv);

    qDebug() << "main #" << "Working directory of Xpiks is:" << QDir::currentPath();

    if (highDpiEnvironmentVariable) {
        qunsetenv(highDpiEnvironmentVariable);
    }

    localLibrary.loadLibraryAsync();

    QString userId = appSettings.value(QLatin1String(Constants::USER_AGENT_ID)).toString();
    userId.remove(QRegExp("[{}-]."));

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::UploadInfoRepository uploadInfoRepository;
    Warnings::WarningsService warningsService;
    Models::SettingsModel settingsModel;
    settingsModel.readAllValues();
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor;
    keywordsSuggestor.setLocalLibrary(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Models::ArtworkUploader artworkUploader(settingsModel.getMaxParallelUploads(), settingsModel.getUploadTimeout());
    SpellCheck::SpellCheckerService spellCheckerService;
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;
    MetadataIO::BackupSaverService metadataSaverService;
    Warnings::WarningsModel warningsModel;
    warningsModel.setSourceModel(&artItemsModel);

    bool updateEnabled=appSettings.value(Constants::UPDATE_SERVICE,true ).toBool();
    Helpers::UpdateService updateService(updateEnabled);

    MetadataIO::MetadataIOCoordinator metadataIOCoordinator;
#ifdef TELEMETRY_ENABLED
    bool telemetryEnabled = appSettings.value(Constants::USER_STATISTICS, true).toBool();
#else
    bool telemetryEnabled = appSettings.value(Constants::USER_STATISTICS, false).toBool();
#endif
    Conectivity::TelemetryService telemetryService(userId, telemetryEnabled);
    Plugins::PluginManager pluginManager;
    Plugins::PluginsWithActionsModel pluginsWithActions;
    pluginsWithActions.setSourceModel(&pluginManager);

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
    commandManager.InjectDependency(&spellCheckerService);
    commandManager.InjectDependency(&spellCheckSuggestionModel);
    commandManager.InjectDependency(&metadataSaverService);
    commandManager.InjectDependency(&telemetryService);
    commandManager.InjectDependency(&updateService);
    commandManager.InjectDependency(&logsModel);
    commandManager.InjectDependency(&localLibrary);
    commandManager.InjectDependency(&metadataIOCoordinator);
    commandManager.InjectDependency(&pluginManager);

    commandManager.ensureDependenciesInjected();

    // other initializations
    secretsManager.setMasterPasswordHash(appSettings.value(Constants::MASTER_PASSWORD_HASH, "").toString());
    uploadInfoRepository.initFromString(appSettings.value(Constants::UPLOAD_HOSTS, "").toString());
    recentDirectorieModel.deserializeFromSettings(appSettings.value(Constants::RECENT_DIRECTORIES, "").toString());

    Helpers::SettingsProvider::getInstance().setSettingsModelInstance(&settingsModel);

    commandManager.connectEntitiesSignalsSlots();

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);

    Helpers::HelpersQmlWrapper helpersQmlWrapper(&commandManager);

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("artItemsModel", &artItemsModel);
    rootContext->setContextProperty("artworkRepository", &artworkRepository);
    rootContext->setContextProperty("combinedArtworks", &combinedArtworksModel);
    rootContext->setContextProperty("appSettings", &appSettings);
    rootContext->setContextProperty("artworkUploader", &artworkUploader);
    rootContext->setContextProperty("uploadInfos", &uploadInfoRepository);
    rootContext->setContextProperty("logsModel", &logsModel);
    rootContext->setContextProperty("secretsManager", &secretsManager);
    rootContext->setContextProperty("undoRedoManager", &undoRedoManager);
    rootContext->setContextProperty("zipArchiver", &zipArchiver);
    rootContext->setContextProperty("keywordsSuggestor", &keywordsSuggestor);
    rootContext->setContextProperty("settingsModel", &settingsModel);
    rootContext->setContextProperty("filteredArtItemsModel", &filteredArtItemsModel);
    rootContext->setContextProperty("helpersWrapper", &helpersQmlWrapper);
    rootContext->setContextProperty("recentDirectories", &recentDirectorieModel);
    rootContext->setContextProperty("updateService", &updateService);
    rootContext->setContextProperty("spellCheckerService", &spellCheckerService);
    rootContext->setContextProperty("spellCheckSuggestionModel", &spellCheckSuggestionModel);
    rootContext->setContextProperty("metadataIOCoordinator", &metadataIOCoordinator);
    rootContext->setContextProperty("pluginManager", &pluginManager);
    rootContext->setContextProperty("pluginsWithActions", &pluginsWithActions);
    rootContext->setContextProperty("warningsModel", &warningsModel);

    engine.addImageProvider("global", globalProvider);
    qDebug() << "main #" << "About to load main view...";
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qDebug() << "main #" << "Main view loaded";

    pluginManager.getUIProvider()->setQmlEngine(&engine);
    QQuickWindow *window = qobject_cast<QQuickWindow*>(engine.rootObjects().at(0));
    pluginManager.getUIProvider()->setRoot(window->contentItem());

//#ifdef QT_DEBUG
//    if (argc > 1) {
//        QStringList pathes;
//        for (int i = 1; i < argc; ++i) {
//            pathes.append(QString(argv[i]));
//        }
//        commandManager.addInitialArtworks(pathes);
//    }
//#endif

    return app.exec();
}
