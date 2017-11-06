/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDir>
#include <QJsonArray>
#include "../Models/artitemsmodel.h"
#include "../Models/imageartwork.h"
#include "../Models/artworksrepository.h"
#include "../Commands/commandmanager.h"
#include "../MetadataIO/artworkssnapshot.h"
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
#define ADDED_AS_DIRECTORY_KEY "asDirectory"

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

    void SessionManager::onAfterRestore() {
        m_Filenames.clear();
        m_Vectors.clear();
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

            bool addedAsDirectory = item->getAddedAsDirectory();
            jsonObject.insert(ADDED_AS_DIRECTORY_KEY, addedAsDirectory);

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
            bool addedAsDirectory =  obj.value(ADDED_AS_DIRECTORY_KEY).toBool();

            if (filePath.isEmpty()) {
                continue;
            }

            m_Filenames.emplace_back(filePath, addedAsDirectory);
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
