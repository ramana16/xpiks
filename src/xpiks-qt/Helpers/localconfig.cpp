/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "localconfig.h"
#include <QDir>
#include <QStandardPaths>

namespace Helpers {
    LocalConfig::LocalConfig() {
    }

    void LocalConfig::initConfig(const QString &configPath) {
        m_FilePath = configPath;
        QFile file(m_FilePath);

        if (file.open(QIODevice::ReadOnly)) {
            QString text = QString::fromUtf8(file.readAll());
            file.close();
            m_Config = QJsonDocument::fromJson(text.toUtf8());
        } else {
            LOG_WARNING << "Opening file" << m_FilePath << "failed";
        }
    }

    bool LocalConfig::saveToFile() {
        bool success = false;
        QFile file(m_FilePath);

        Q_ASSERT(!m_FilePath.isEmpty());

        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_Config.toJson(QJsonDocument::Indented));
            file.close();
            success = true;
        } else {
            LOG_WARNING << "Opening file" << m_FilePath << "failed";
        }

        return success;
    }

    void LocalConfig::dropConfig() {
        m_Config = QJsonDocument();
        Q_ASSERT(m_Config.isEmpty());
    }
}

