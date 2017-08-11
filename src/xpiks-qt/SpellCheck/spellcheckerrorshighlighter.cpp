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

namespace SpellCheck {
    SpellCheckErrorsHighlighter::SpellCheckErrorsHighlighter(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                             SpellCheckErrorsInfo *errorsInfo) :
        QSyntaxHighlighter(document),
        m_SpellCheckErrors(errorsInfo),
        m_ColorsModel(colorsModel)
    {
    }

    void SpellCheckErrorsHighlighter::highlightBlock(const QString &text) {
        if (!m_SpellCheckErrors->anyError()) { return; }

        QColor destructiveColor = m_ColorsModel->destructiveColor();

        Helpers::foreachWord(text,
                             [this](const QString &word) {
            return this->m_SpellCheckErrors->hasWrongSpelling(word); },
        [this, &destructiveColor](int start, int length, const QString &) {
            this->setFormat(start, length, destructiveColor); }
        );
    }
}
