/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "translationquery.h"

namespace Translation {
    TranslationQuery::TranslationQuery(const QString &query, QObject *parent) :
        QObject(parent),
        m_Query(query),
        m_Success(false)
    {
    }

    void TranslationQuery::setTranslation(const QString &translation) {
        if (!translation.isEmpty()) {
            m_Translation = translation;
            m_Success = true;
            emit translationAvailable();
        }
    }

    void TranslationQuery::setFailed() {
        m_Success = false;
        emit translationAvailable();
    }
}
