/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WARNINGSSETTINGSMODEL_H
#define WARNINGSSETTINGSMODEL_H

#include <QJsonDocument>
#include <QJsonArray>
#include "../Models/abstractconfigupdatermodel.h"

namespace Warnings {
    class WarningsSettingsModel:
        public Models::AbstractConfigUpdaterModel
    {
    Q_OBJECT

    public:
        WarningsSettingsModel();
        void initializeConfigs();

        const QString &getAllowedFilenameCharacters() const { return m_AllowedFilenameCharacters; }
        double getMinMegapixels() const { return m_MinMegapixels; }
        double getMaxFilesizeMB() const { return m_MaxFilesizeMB; }
        int getMinKeywordsCount() const { return m_MinKeywordsCount; }
        int getMaxKeywordsCount() const { return m_MaxKeywordsCount; }
        int getMinWordsCount() const { return m_MinWordsCount; }
        int getMaxDescriptionLength() const { return m_MaxDescriptionLength; }

        // AbstractConfigUpdaterModel interface
    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) override;
        virtual bool processLocalConfig(const QJsonDocument &document) override;

        // CompareValuesJson interface
    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2) override;

    private:
        QString m_AllowedFilenameCharacters;
        double m_MinMegapixels;
        double m_MaxFilesizeMB;
        int m_MinKeywordsCount;
        int m_MaxKeywordsCount;
        int m_MinWordsCount;
        int m_MaxDescriptionLength;
    };
}
#endif // WARNINGSSETTINGSMODEL_H
