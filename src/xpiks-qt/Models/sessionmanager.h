/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QJsonObject>
#include <QMutex>
#include <memory>
#include "../Helpers/localconfig.h"
#include "../Common/baseentity.h"

namespace MetadataIO {
    class ArtworkSessionSnapshot;
    class SessionSnapshot;
}

namespace Models {
    class ArtworkMetadata;
}

namespace Models {
    class SessionManager: public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        SessionManager();
        virtual ~SessionManager() {}

    public:
        void initialize();
        void onBeforeRestore();
        void onAfterRestore();
        void saveToFile(std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > &filesSnapshot,
                        const QStringList &directoriesSnapshot);
        void readSessionFromFile();

    private:
        void parseFiles();
        void parseDirectories();

    public:
        const QStringList &getFilenames() const { return m_Filenames; }
        const QStringList &getVectors() const { return m_Vectors; }
        const QStringList &getFullDirectories() const { return m_FullDirectories; }

#ifdef INTEGRATION_TESTS
        int itemsCount() const;
        void clearSession();
#endif

    private:
        inline void setValue(const char *key, const QJsonValue &value) {
            m_SessionJson.insert(QLatin1String(key), value);
        }

        inline QJsonValue value(const char *key, const QJsonValue &defaultValue = QJsonValue()) const {
            QJsonValue value = m_SessionJson.value(QLatin1String(key));

            if (value.isUndefined() || value.isNull()) {
                return defaultValue;
            }

            return value;
        }

    private:
        Helpers::LocalConfig m_Config;
        QString m_LocalConfigPath;
        QJsonObject m_SessionJson;
        QMutex m_Mutex;
        QStringList m_Filenames;
        QStringList m_Vectors;
        QStringList m_FullDirectories;
        volatile bool m_CanRestore;
    };
}

#endif // SESSIONMANAGER_H
