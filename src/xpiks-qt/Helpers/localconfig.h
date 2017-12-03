/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QString>
#include <QJsonDocument>
#include <QFile>
#include "../Common/defines.h"

namespace Helpers {
    class LocalConfig {
    public:
        LocalConfig();

    public:
        QJsonDocument& getConfig() { return m_Config; }
        const QJsonDocument& getConfig() const { return m_Config; }
        void setConfig(const QJsonDocument &config) { m_Config = config; }
        void setPath(const QString &filePath) { m_FilePath = filePath; }
        void initConfig(const QString &configPath);
        bool saveToFile();
        void dropConfig();

    private:
        QString m_FilePath;
        QJsonDocument m_Config;
    };

    class LocalConfigDropper {
    public:
        LocalConfigDropper(LocalConfig *config):
            m_Config(config)
        {
        }

        virtual ~LocalConfigDropper() {
            if (m_Config != nullptr) {
                m_Config->dropConfig();
            }
        }

    private:
        LocalConfig *m_Config;
    };
}

#endif // LOCALCONFIG_H
