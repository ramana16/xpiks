/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UPLOADCONTEXT
#define UPLOADCONTEXT

#include <QString>
#include "../Common/defines.h"

namespace Models {
    class ProxySettings;
}

namespace Conectivity {
    class UploadContext {
    public:
        ~UploadContext() {
            LOG_DEBUG << "destructor for host" << m_Host;
        }

    public:
        QString m_Host;
        QString m_Username;
        QString m_Password;
        QString m_DirForVectors;
        QString m_DirForImages;
        bool m_UsePassiveMode;
        bool m_UseEPSV;
        int m_RetriesCount;
        int m_TimeoutSeconds;
        bool m_UseProxy;
        bool m_VerboseLogging;
        Models::ProxySettings *m_ProxySettings;
    };
}

#endif // UPLOADCONTEXT

