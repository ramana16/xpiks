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
        QString m_Stem;
        bool m_IsCorrect;

        WordAnalysisResult():
            m_Stem(), m_IsCorrect(true)
        {}

        WordAnalysisResult(const QString & stem, bool spellCheck):
            m_Stem(stem), m_IsCorrect(spellCheck)
        {}

        void reset() {
            m_Stem.clear();
            m_IsCorrect = true;
        }

    };

    struct KeywordSpellInfo {
        QStringList m_Stems;
        bool m_IsCorrect;

        KeywordSpellInfo():
            m_Stems(), m_IsCorrect(true)
        {}

        void reset()
        {
            m_Stems.clear();
            m_IsCorrect = true;
        }

    };
}

#endif // WORDANALYSISRESULT_H
