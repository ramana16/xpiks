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

#include <QDir>
#include "../Models/artitemsmodel.h"
#include "../Models/imageartwork.h"
#include "../Models/artworksrepository.h"
#include "../Commands/commandmanager.h"
#include "../MetadataIO/artworkmetadatasnapshot.h"
#include "sessionmanager.h"

#ifdef QT_DEBUG
    #ifdef INTEGRATION_TESTS
        #define SESSION_FILE "integration_session.json"
    #else
        #define SESSION_FILE "debug_session.json"
    #endif
#else
    #define SESSION_FILE "session.json"
#endif

#define OPENED_FILES_KEY "openedFiles"
#define FILE_KEY "file"
#define VECTOR_KEY "vector"

namespace Models {
    SessionManager::SessionManager():
        QObject(),
        m_CanRestore(false)
    {
    }

    void SessionManager::initialize() {
        LOG_DEBUG << "#";

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            m_LocalConfigPath = appDataDir.filePath(SESSION_FILE);
        } else {
            m_LocalConfigPath = SESSION_FILE;
        }

        m_Config.setPath(m_LocalConfigPath);
    }

    void SessionManager::onBeforeRestore() {
        LOG_DEBUG << "#";
        m_CanRestore = true;
    }

    void SessionManager::saveToFile(std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > &snapshot) {
        if (!m_CanRestore) {
            LOG_INFO << "Session hasn't been initialized yet. Exiting...";
            return;
        }

        LOG_INFO << snapshot.size() << "artwork(s)";

        QJsonArray filesList;

        for (auto &item: snapshot) {
            QJsonObject jsonObject;

            auto &filePath = item->getArtworkFilePath();
            jsonObject.insert(FILE_KEY, filePath);

            auto &vectorPath = item->getAttachedVectorPath();
            if (!vectorPath.isEmpty()) {
                jsonObject.insert(VECTOR_KEY, vectorPath);
            }

            filesList.append(jsonObject);
        }

        setValue(OPENED_FILES_KEY, filesList);

        QJsonDocument doc;
        doc.setObject(m_SessionJson);

        m_Config.setConfig(doc);

        Helpers::LocalConfigDropper dropper(&m_Config);
        Q_UNUSED(dropper);

        QMutexLocker locker(&m_Mutex);
        Q_UNUSED(locker);

        m_Config.saveToFile();
    }

    void SessionManager::readSessionFromFile() {
        LOG_DEBUG << "#";

        m_Config.initConfig(m_LocalConfigPath);
        Helpers::LocalConfigDropper dropper(&m_Config);
        Q_UNUSED(dropper);

        QJsonDocument &doc = m_Config.getConfig();
        if (doc.isObject()) {
            m_SessionJson = doc.object();
        } else {
            LOG_WARNING << "JSON document doesn't contain an object";
            return;
        }

        QJsonArray filesArray = value(OPENED_FILES_KEY).toArray();

        if (filesArray.isEmpty()) {
            LOG_DEBUG << "The session is empty";
            return;
        }

        m_Filenames.reserve(filesArray.size());
        m_Vectors.reserve(filesArray.size()/2);

        for (const auto &item: filesArray) {
            QJsonObject obj = item.toObject();
            QString filePath = obj.value(FILE_KEY).toString().trimmed();

            if (filePath.isEmpty()) {
                continue;
            }

            m_Filenames.append(filePath);

            QString vectorPath = obj.value(VECTOR_KEY).toString().trimmed();
            if (!vectorPath.isEmpty()) {
                m_Vectors.append(vectorPath);
            }
        }
    }

#ifdef INTEGRATION_TESTS
    int SessionManager::itemsCount() const {
        const QJsonArray &filesArray = value(OPENED_FILES_KEY).toArray();
        return filesArray.size();
    }

    void SessionManager::clearSession() {
        setValue(OPENED_FILES_KEY, QJsonArray());
        m_Filenames.clear();
        m_Vectors.clear();
    }
#endif
}
