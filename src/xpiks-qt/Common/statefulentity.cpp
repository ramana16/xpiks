/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "statefulentity.h"
#include <QDir>
#include "../Helpers/constants.h"
#include "../Helpers/filehelpers.h"

namespace Common {
    StatefulEntity::StatefulEntity(const QString &stateName):
        m_StateName(stateName)
    {
        Q_ASSERT(!stateName.endsWith(".json", Qt::CaseInsensitive));
    }

    void StatefulEntity::initState() {
        LOG_DEBUG << m_StateName;
        QString localConfigPath;

#if defined(QT_DEBUG)
        QString filename = QString("debug_%1.json").arg(m_StateName);
#elif defined(INTEGRATION_TESTS)
        QString filename = QString("tests_%1.json").arg(m_StateName);
#else
        QString filename = QString("%1.json").arg(m_StateName);
#endif

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            const QString statesPath = QDir::cleanPath(appDataPath + QDir::separator() + Constants::STATES_DIR);
            QDir statesDir(statesPath);
            Q_ASSERT(statesDir.exists());
            localConfigPath = statesDir.filePath(filename);
        } else {
            localConfigPath = filename;
        }

        m_StateConfig.initConfig(localConfigPath);

        QJsonDocument &doc = m_StateConfig.getConfig();
        if (doc.isObject()) {
            m_StateJson = doc.object();
        }
    }

    void StatefulEntity::syncState() {
        LOG_DEBUG << m_StateName;

        // do not use dropper
        // Helpers::LocalConfigDropper dropper(&m_StateConfig);

        QJsonDocument doc;
        doc.setObject(m_StateJson);

        m_StateConfig.setConfig(doc);
        m_StateConfig.saveToFile();
    }
}
