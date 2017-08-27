/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SEARCHQUERY_H
#define SEARCHQUERY_H

#include <QObject>
#include <QStringList>
#include "../Common/flags.h"

namespace Suggestion {
    enum QueryFlags {
        AllImages = 1 << 0,
        Photos = 1 << 1,
        Vectors = 1 << 2,
        Illustrations = 1 << 3,
        Videos = 1 << 4
    };

    struct SearchQuery {
        SearchQuery():
            m_Flags(0),
            m_MaxResults(0)
        {}

        SearchQuery(const QString &searchTerm, int resultType) {
            // "All Images"
            // "Photos"
            // "Vectors"
            // "Illustrations"

            m_SearchTerms = searchTerm.split(QChar::Space, QString::SkipEmptyParts);

            switch (resultType) {
            case 0:
                Common::SetFlag(m_Flags, AllImages);
                break;
            case 1:
                Common::SetFlag(m_Flags, Photos);
                break;
            case 2:
                Common::SetFlag(m_Flags, Vectors);
                break;
            case 3:
                Common::SetFlag(m_Flags, Illustrations);
                break;
            case 4:
                Common::SetFlag(m_Flags, Videos);
                break;
            default:
                break;
            }
        }

        QStringList m_SearchTerms;
        Common::flag_t m_Flags;
        int m_MaxResults;
    };
}

#endif // SEARCHQUERY_H
