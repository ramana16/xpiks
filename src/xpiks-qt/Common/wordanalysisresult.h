/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WORDANALYSISRESULT_H
#define WORDANALYSISRESULT_H
#include <QStringList>

namespace Common {
    struct WordAnalysisResult {
        WordAnalysisResult():
            m_IsCorrect(true),
            m_HasDuplicates(false)
        { }

        WordAnalysisResult(const QString &stem, bool isCorrect):
            m_Stem(stem),
            m_IsCorrect(isCorrect),
            m_HasDuplicates(false)
        { }

        WordAnalysisResult(const QString &stem, bool isCorrect, bool hasDuplicates):
            m_Stem(stem),
            m_IsCorrect(isCorrect),
            m_HasDuplicates(hasDuplicates)
        { }

        void reset() {
            m_Stem.clear();
            m_IsCorrect = true;
            m_HasDuplicates = false;
        }

        QString m_Stem;
        volatile bool m_IsCorrect;
        volatile bool m_HasDuplicates;
    };
}

#endif // WORDANALYSISRESULT_H
