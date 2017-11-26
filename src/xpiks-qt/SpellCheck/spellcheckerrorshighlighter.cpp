/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "spellcheckerrorshighlighter.h"
#include <QColor>
#include <QSet>
#include <QString>
#include "spellcheckiteminfo.h"
#include "../QMLExtensions/colorsmodel.h"
#include "../Helpers/stringhelper.h"
#include "../Common/defines.h"

namespace SpellCheck {
    SpellCheckErrorsHighlighter::SpellCheckErrorsHighlighter(QTextDocument *document,
                                                             QMLExtensions::ColorsModel *colorsModel,
                                                             SpellCheckErrorsInfo *errorsInfo) :
        QSyntaxHighlighter(document),
        m_SpellCheckErrors(errorsInfo),
        m_ColorsModel(colorsModel)
    {
    }

    SpellCheckErrorsHighlighter::~SpellCheckErrorsHighlighter() {
        // LOG_FOR_DEBUG << "destroyed";
    }

    void SpellCheckErrorsHighlighter::highlightBlock(const QString &text) {
        if (!m_SpellCheckErrors->anyError() && !m_SpellCheckErrors->anyDuplicate()) { return; }

        QColor destructiveColor = m_ColorsModel->destructiveColor();
        QTextCharFormat wrongSpellingFormat;
        /*
        // TODO: there a bug in Qt https://bugreports.qt.io/browse/QTBUG-39617 so this is not working
        wrongSpellingFormat.setFontUnderline(true);
        wrongSpellingFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        wrongSpellingFormat.setUnderlineColor(destructiveColor);*/
        wrongSpellingFormat.setForeground(QBrush(destructiveColor));

        QTextCharFormat duplicatesFormat;
        duplicatesFormat.setFontUnderline(true);
        duplicatesFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

        const QString textLower = text.toLower();

        Helpers::foreachWord(textLower,
                             [this](const QString &) { return true; },
        [this, &duplicatesFormat, &wrongSpellingFormat](int start, int length, const QString &word) {
            if (this->m_SpellCheckErrors->hasWrongSpelling(word)) {
                this->setFormat(start, length, wrongSpellingFormat);
            } else if (this->m_SpellCheckErrors->hasDuplicates(word)) {
                this->setFormat(start, length, duplicatesFormat);
            }
        });
    }
}
