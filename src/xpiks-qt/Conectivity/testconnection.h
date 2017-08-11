/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TESTCONNECTION_H
#define TESTCONNECTION_H

#include <QString>

namespace Conectivity {
    class UploadContext;

    struct ContextValidationResult {
        ContextValidationResult(): m_Result(false) {}
        ContextValidationResult(const ContextValidationResult &copy):
            m_Host(copy.m_Host),
            m_Result(copy.m_Result)
        {}

        QString m_Host;
        bool m_Result;
    };

    ContextValidationResult isContextValid(UploadContext *context);
}

#endif // TESTCONNECTION_H
