/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        void saveToFile(std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > &snapshot);
        void readSessionFromFile();

    public:
        const QStringList &getFilenames() const { return m_Filenames; }
        const QStringList &getVectors() const { return m_Vectors; }

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
        volatile bool m_CanRestore;
    };
}

#endif // SESSIONMANAGER_H
