/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QString>
#include "../Common/version.h"

#if defined(Q_OS_DARWIN)
#  define UPDATE_JSON_URL_SUFFIX "update-osx.json"
#elif defined(Q_OS_WIN64)
#  define UPDATE_JSON_URL_SUFFIX "update-windows.json"
#elif defined(Q_OS_WIN32)
#  define UPDATE_JSON_URL_SUFFIX "update-windows-32.json"
#else
#  define UPDATE_JSON_URL_SUFFIX "update.json"
#endif

namespace Connectivity {
    class ApiManager
    {
    public:
        static ApiManager& getInstance()
        {
            static ApiManager instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    public:
        QString getUpdateAddr() const {
            return m_BaseUrl + QLatin1String(UPDATE_JSON_URL_SUFFIX);
        }

        QString getDefaultUpdateAddr() const {
            return m_BaseUrl + "update.json";
        }

        QString getStocksACSourceAddr() const {
            return m_BaseUrl + "stocks_ftp.json";
        }

        QString getWarningSettingsAddr() const {
            return m_BaseUrl + "warnings_settings.json";
        }

        QString getPresetsSourceAddr() const {
            return m_BaseUrl + "presets.json";
        }

        QString getSwitcherAddr() {
            QString filename = QString("switches_%1.json").arg(XPIKS_API_VERSION_STRING);
            return m_BaseUrl + filename;
        }

    private:
        ApiManager() {
#ifdef QT_DEBUG
            m_BaseUrl = "https://ribtoks.github.io/xpiks/api/dev/";
#else
            m_BaseUrl = "https://ribtoks.github.io/xpiks/api/v1/";
#endif
        }

        ApiManager(ApiManager const&);
        void operator=(ApiManager const&);

    private:
        QString m_BaseUrl;
    };
}

#endif // APIMANAGER_H
