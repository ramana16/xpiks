/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYWORD_H
#define KEYWORD_H

#include <QString>
#include <QStringList>

namespace Common {
    struct Keyword {
        Keyword():
            m_IsCorrect(true),
            m_HasDuplicates(false)
        { }

        Keyword(const QString &text):
            m_Value(text),
            m_IsCorrect(true),
            m_HasDuplicates(false)
        { }

        Keyword(const Keyword &other):
            m_Value(other.m_Value),
            m_IsCorrect(other.m_IsCorrect),
            m_HasDuplicates(other.m_HasDuplicates)
        { }

        void resetSpelling() {
            m_IsCorrect = true;
            m_HasDuplicates = false;
        }

        QString m_Value;
        volatile bool m_IsCorrect;
        volatile bool m_HasDuplicates;

        // we like aligned structs
        char m_Dummy1 = 0;
        char m_Dummy2 = 0;
    };

    struct KeywordItem {
        KeywordItem(const QString &word, size_t index):
            m_Word(word),
            m_Index(index)
        { }

        KeywordItem(const QString &word, size_t index, const QString &origin):
            m_Word(word),
            m_Index(index),
            m_OriginKeyword(origin)
        { }

        bool isPartOfAKeyword() const { return m_OriginKeyword.isEmpty(); }

        QString m_Word;
        size_t m_Index;
        // empty if word == keyword
        QString m_OriginKeyword;
    };
}

#endif // KEYWORD_H
