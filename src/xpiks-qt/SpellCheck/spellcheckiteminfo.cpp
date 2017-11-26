/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "spellcheckiteminfo.h"
#include <QReadLocker>
#include <QWriteLocker>
#include <QReadWriteLock>
#include "spellcheckerrorshighlighter.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/basickeywordsmodel.h"
#include "../QMLExtensions/colorsmodel.h"

namespace SpellCheck {
    bool SpellCheckErrorsInfo::hasWrongSpelling(const QString &word) {
        Q_ASSERT(word == word.toLower());
        QReadLocker readLocker(&m_ErrorsLock);
        Q_UNUSED(readLocker);

        return m_WordsWithErrors.contains(word);
    }

    bool SpellCheckErrorsInfo::hasDuplicates(const QString &word) {
        Q_ASSERT(word == word.toLower());
        QReadLocker readLocker(&m_DuplicatesLock);
        Q_UNUSED(readLocker);

        return m_WordsWithDuplicates.contains(word);
    }

    void SpellCheckErrorsInfo::setErrorWords(const QSet<QString> &errors) {
        QWriteLocker writeLocker(&m_ErrorsLock);
        Q_UNUSED(writeLocker);

#ifdef QT_DEBUG
        for (auto &word: errors) { Q_ASSERT(word == word.toLower()); }
#endif
        /*m_WordsWithErrors.clear();*/ m_WordsWithErrors.unite(errors);
    }

    void SpellCheckErrorsInfo::setDuplicates(const QSet<QString> &duplicates) {
        QWriteLocker writeLocker(&m_DuplicatesLock);
        Q_UNUSED(writeLocker);

#ifdef QT_DEBUG
        for (auto &word: duplicates) { Q_ASSERT(word == word.toLower()); }
#endif

        m_WordsWithDuplicates.clear();
        m_WordsWithDuplicates.unite(duplicates);
    }

    bool SpellCheckErrorsInfo::removeWordFromErrors(const QString &word) {
        Q_ASSERT(word == word.toLower());
        QWriteLocker writeLocker(&m_ErrorsLock);
        Q_UNUSED(writeLocker);

        return m_WordsWithErrors.remove(word.toLower());
    }

    bool SpellCheckErrorsInfo::anyError() {
        QReadLocker readLocker(&m_ErrorsLock);
        Q_UNUSED(readLocker);

        return !m_WordsWithErrors.isEmpty();
    }

    bool SpellCheckErrorsInfo::anyDuplicate() {
        QReadLocker readLocker(&m_DuplicatesLock);
        Q_UNUSED(readLocker);

        return !m_WordsWithDuplicates.isEmpty();
    }

    void SpellCheckErrorsInfo::clear() {
        {
            QWriteLocker writeLocker(&m_ErrorsLock);
            Q_UNUSED(writeLocker);

            m_WordsWithErrors.clear();
        }

        {
            QWriteLocker writeLocker(&m_DuplicatesLock);
            Q_UNUSED(writeLocker);

            m_WordsWithDuplicates.clear();
        }
    }

    QStringList SpellCheckErrorsInfo::toList() {
        QReadLocker readLocker(&m_ErrorsLock);
        Q_UNUSED(readLocker);

        return QStringList::fromSet(m_WordsWithErrors);
    }

    void SpellCheckItemInfo::setDescriptionErrors(const QSet<QString> &errors) {
        m_DescriptionErrors.setErrorWords(errors);
    }

    void SpellCheckItemInfo::setTitleErrors(const QSet<QString> &errors) {
        m_TitleErrors.setErrorWords(errors);
    }

    void SpellCheckItemInfo::setDescriptionDuplicates(const QSet<QString> &duplicates) {
        m_DescriptionErrors.setDuplicates(duplicates);
    }

    void SpellCheckItemInfo::setTitleDuplicates(const QSet<QString> &duplicates) {
        m_TitleErrors.setDuplicates(duplicates);
    }

    void SpellCheckItemInfo::removeWordsFromErrors(const QStringList &words) {
        LOG_DEBUG << "#";

        for (const QString &word: words) {
            m_TitleErrors.removeWordFromErrors(word.toLower());
            m_DescriptionErrors.removeWordFromErrors(word.toLower());
        }
    }

    QSyntaxHighlighter *SpellCheckItemInfo::createHighlighterForDescription(QTextDocument *document,
                                                                            QMLExtensions::ColorsModel *colorsModel,
                                                                            Common::BasicMetadataModel *basicModel) {
        // is freed by the document
#ifndef CORE_TESTS
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_DescriptionErrors);
        if (basicModel != nullptr) {
            QObject::connect(basicModel, &Common::BasicMetadataModel::descriptionSpellingChanged,
                             highlighter, &SpellCheckErrorsHighlighter::rehighlight);
        }

        QObject::connect(colorsModel, &QMLExtensions::ColorsModel::themeChanged,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);

        return highlighter;
#else
        Q_UNUSED(document);
        Q_UNUSED(colorsModel);
        Q_UNUSED(basicModel);
        return nullptr;
#endif
    }

    QSyntaxHighlighter *SpellCheckItemInfo::createHighlighterForTitle(QTextDocument *document,
                                                                      QMLExtensions::ColorsModel *colorsModel,
                                                                      Common::BasicMetadataModel *basicModel) {
#ifndef CORE_TESTS
        // is freed by the document
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_TitleErrors);
        if (basicModel != nullptr) {
            QObject::connect(basicModel, &Common::BasicMetadataModel::titleSpellingChanged,
                             highlighter, &SpellCheckErrorsHighlighter::rehighlight);
        }

        QObject::connect(colorsModel, &QMLExtensions::ColorsModel::themeChanged,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);

        return highlighter;
#else
        Q_UNUSED(document);
        Q_UNUSED(colorsModel);
        Q_UNUSED(basicModel);
        return nullptr;
#endif
    }
}
