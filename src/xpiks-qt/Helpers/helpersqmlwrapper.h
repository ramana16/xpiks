/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HELPERSQMLWRAPPER_H
#define HELPERSQMLWRAPPER_H

#include <QObject>
#include <QString>
#include "../Common/baseentity.h"

namespace Commands {
    class CommandManager;
}

class QQuickWindow;
#ifdef Q_OS_WIN
class QWinTaskbarButton;
#endif

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class HelpersQmlWrapper : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool pluginsAvailable READ getPluginsAvailable CONSTANT)
        Q_PROPERTY(bool isUpdateDownloaded READ getIsUpdateDownloaded NOTIFY updateDownloadedChanged)
    public:
        HelpersQmlWrapper(QMLExtensions::ColorsModel *colorsModel);

    public:
        Q_INVOKABLE bool isKeywordValid(const QString &keyword) const;
        Q_INVOKABLE QString sanitizeKeyword(const QString &keyword) const;
        Q_INVOKABLE void afterConstruction();
        Q_INVOKABLE void beforeDestruction();
        Q_INVOKABLE void revealLogFile();
        Q_INVOKABLE void reportOpen();
        Q_INVOKABLE void setProgressIndicator(QQuickWindow *window);
        Q_INVOKABLE void turnTaskbarProgressOn();
        Q_INVOKABLE void setTaskbarProgress(double value);
        Q_INVOKABLE void turnTaskbarProgressOff();
        Q_INVOKABLE void removeUnavailableFiles();
        Q_INVOKABLE void revealArtworkFile(const QString &path);
        Q_INVOKABLE bool isVector(const QString &path) const;
        Q_INVOKABLE QString toImagePath(const QString &path) const;
        Q_INVOKABLE void setUpgradeConsent();
        Q_INVOKABLE void upgradeNow();
        Q_INVOKABLE QString getAssetForTheme(const QString &assetName, int themeIndex) const;

    public:
        void requestCloseApplication() { emit globalCloseRequested(); }

    public:
        Q_INVOKABLE QObject *getLogsModel();
        Q_INVOKABLE QObject *getFtpACList();
        Q_INVOKABLE QObject *getArtworkUploader();
        Q_INVOKABLE QObject *getZipArchiver();
        Q_INVOKABLE QObject *getSpellCheckerService();
        Q_INVOKABLE QObject *getDeleteKeywordsModel();
        Q_INVOKABLE QObject *getUploadInfos();
        Q_INVOKABLE QObject *getSpellCheckSuggestionsModel();

    public:
        bool getPluginsAvailable() const;
        bool getIsUpdateDownloaded() const { return m_IsUpdateDownloaded; }
        bool getUpgradeConsent() const { return m_HaveUpgradeConsent; }

   private:
        void revealFile(const QString &path);

    public slots:
        void updateIsDownloaded(QString pathToUpdate);

    signals:
        void globalCloseRequested();
        void globalBeforeDestruction();
        void updateAvailable(QString updateLink);
        void updateDownloaded();
        void updateDownloadedChanged(bool value);
        void upgradeInitiated();

    private:
#ifdef Q_OS_WIN
        QWinTaskbarButton *m_TaskbarButton;
        bool m_WinTaskbarButtonApplicable;
#endif
        bool m_IsUpdateDownloaded;
        bool m_HaveUpgradeConsent;
        QString m_PathToUpdate;
        QMLExtensions::ColorsModel *m_ColorsModel;
    };
}

#endif // HELPERSQMLWRAPPER_H
