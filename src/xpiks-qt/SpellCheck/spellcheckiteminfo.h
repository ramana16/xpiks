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

class QSyntaxHighlighter;

namespace SpellCheck {
    class SpellCheckErrorsHighlighter;

    class SpellCheckErrorsInfo
    {
    public:
        bool hasWrongSpelling(const QString &word);
        bool hasDuplicates(const QString &word);
        void setErrorWords(const QSet<QString> &errors);
        void setDuplicates(const QSet<QString> &duplicates);
        bool removeWordFromSet(const QString &word);
        bool anyError();
        bool anyDuplicate();
        void clear();
        QStringList toList();

    private:
        QSet<QString> m_WordsWithErrors;
        QSet<QString> m_WordsWithDuplicates;
        QReadWriteLock m_ErrorsLock;
        QReadWriteLock m_DuplicatesLock;
    };

    class SpellCheckItemInfo
    {
    public:
        void setDescriptionErrors(const QSet<QString> &errors);
        void setTitleErrors(const QSet<QString> &errors);
        void setDescriptionDuplicates(const QSet<QString> &duplicates);
        void setTitleDuplicates(const QSet<QString> &duplicates);

        void removeWordsFromErrors(const QStringList &words);
        QSyntaxHighlighter *createHighlighterForDescription(QTextDocument *document,
                                                            QMLExtensions::ColorsModel *colorsModel,
                                                            Common::BasicKeywordsModel *babasicKeywordsModel);
        QSyntaxHighlighter *createHighlighterForTitle(QTextDocument *document,
                                                      QMLExtensions::ColorsModel *colorsModel,
                                                      Common::BasicKeywordsModel *babasicKeywordsModel);

        bool hasDescriptionError(const QString &word) { return m_DescriptionErrors.hasWrongSpelling(word); }
        bool hasTitleError(const QString &word) { return m_TitleErrors.hasWrongSpelling(word); }
        bool hasTitleDuplicate(const QString &word) { return m_TitleErrors.hasDuplicates(word); }
        bool hasDescriptionDuplicate(const QString &word) { return m_DescriptionErrors.hasDuplicates(word); }
        void clear() { m_DescriptionErrors.clear(); m_TitleErrors.clear(); }
        bool anyTitleDuplicates() { return m_TitleErrors.anyDuplicate(); }
        bool anyDescriptionDuplicates() { return m_DescriptionErrors.anyDuplicate(); }

        SpellCheckErrorsInfo *getTitleErrors() { return &m_TitleErrors; }
        SpellCheckErrorsInfo *getDescriptionErrors() { return &m_DescriptionErrors; }

    private:
        SpellCheckErrorsInfo m_DescriptionErrors;
        SpellCheckErrorsInfo m_TitleErrors;
    };
}

#endif // SPELLCHECKITEMINFO_H
