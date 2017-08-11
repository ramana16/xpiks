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
#include "../Common/basickeywordsmodel.h"
#include "../QMLExtensions/colorsmodel.h"

namespace SpellCheck {
    bool SpellCheckErrorsInfo::hasWrongSpelling(const QString &word) {
        QReadLocker readLocker(&m_ErrorsLock);

        Q_UNUSED(readLocker);

        return m_WordsWithErrors.contains(word.toLower());
    }

    void SpellCheckErrorsInfo::setErrorWords(const QSet<QString> &errors) {
        QWriteLocker writeLocker(&m_ErrorsLock);

        Q_UNUSED(writeLocker);

        /*m_WordsWithErrors.clear();*/ m_WordsWithErrors.unite(errors);
    }

    bool SpellCheckErrorsInfo::removeWordFromSet(const QString &word) {
        QWriteLocker writeLocker(&m_ErrorsLock);

        Q_UNUSED(writeLocker);
        return m_WordsWithErrors.remove(word.toLower());
    }

    bool SpellCheckErrorsInfo::anyError() {
        QReadLocker readLocker(&m_ErrorsLock);

        Q_UNUSED(readLocker);

        return !m_WordsWithErrors.isEmpty();
    }

    void SpellCheckErrorsInfo::clear() {
        QWriteLocker writeLocker(&m_ErrorsLock);

        Q_UNUSED(writeLocker);

        m_WordsWithErrors.clear();
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

    void SpellCheckItemInfo::removeWordsFromErrors(const QStringList &words) {
        LOG_DEBUG << "#";
        for (const QString &word: words) {
            m_TitleErrors.removeWordFromSet(word);
            m_DescriptionErrors.removeWordFromSet(word);
        }
    }

    void SpellCheckItemInfo::createHighlighterForDescription(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                             Common::BasicKeywordsModel *basicKeywordsModel) {
        // is freed by the document
#ifndef CORE_TESTS
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_DescriptionErrors);
        QObject::connect(basicKeywordsModel, &Common::BasicKeywordsModel::spellCheckResultsReady,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);
        QObject::connect(colorsModel, &QMLExtensions::ColorsModel::themeChanged,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);
#else
        Q_UNUSED(document);
        Q_UNUSED(colorsModel);
        Q_UNUSED(basicKeywordsModel);
#endif
    }

    void SpellCheckItemInfo::createHighlighterForTitle(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                       Common::BasicKeywordsModel *basicKeywordsModel) {
#ifndef CORE_TESTS
        // is freed by the document
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_TitleErrors);
        QObject::connect(basicKeywordsModel, &Common::BasicKeywordsModel::spellCheckResultsReady,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);
        QObject::connect(colorsModel, &QMLExtensions::ColorsModel::themeChanged,
                         highlighter, &SpellCheckErrorsHighlighter::rehighlight);
#else
        Q_UNUSED(document);
        Q_UNUSED(colorsModel);
        Q_UNUSED(basicKeywordsModel);
#endif
    }
}
