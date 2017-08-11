/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKITEMINFO_H
#define SPELLCHECKITEMINFO_H

#include <QSet>
#include <QString>
#include <QObject>
#include <QStringList>
#include <QTextDocument>
#include <QReadWriteLock>

namespace Common {
    class BasicKeywordsModel;
}

namespace QMLExtensions {
    class ColorsModel;
}

namespace SpellCheck {
    class SpellCheckErrorsHighlighter;

    class SpellCheckErrorsInfo
    {
    public:
        bool hasWrongSpelling(const QString &word);
        void setErrorWords(const QSet<QString> &errors);
        bool removeWordFromSet(const QString &word);
        bool anyError();
        void clear();
        QStringList toList();

    private:
        QSet<QString> m_WordsWithErrors;
        QReadWriteLock m_ErrorsLock;
    };

    class SpellCheckItemInfo
    {
    public:
        void setDescriptionErrors(const QSet<QString> &errors);
        void setTitleErrors(const QSet<QString> &errors);
        void removeWordsFromErrors(const QStringList &words);
        void createHighlighterForDescription(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                             Common::BasicKeywordsModel *basicKeywordsModel);
        void createHighlighterForTitle(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                       Common::BasicKeywordsModel *basicKeywordsModel);

        bool hasDescriptionError(const QString &word) { return m_DescriptionErrors.hasWrongSpelling(word); }
        bool hasTitleError(const QString &word) { return m_TitleErrors.hasWrongSpelling(word); }
        void clear() { m_DescriptionErrors.clear(); m_TitleErrors.clear(); }

    private:
        SpellCheckErrorsInfo m_DescriptionErrors;
        SpellCheckErrorsInfo m_TitleErrors;
    };
}

#endif // SPELLCHECKITEMINFO_H
