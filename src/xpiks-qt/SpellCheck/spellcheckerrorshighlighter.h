/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKERRORSHIGHLIGHTER_H
#define SPELLCHECKERRORSHIGHLIGHTER_H

#include <QtGui>
#include <QSet>
#include <QString>

namespace QMLExtensions {
    class ColorsModel;
}

namespace SpellCheck {
    class SpellCheckErrorsInfo;

    class SpellCheckErrorsHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT
    public:
        SpellCheckErrorsHighlighter(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                    SpellCheckErrorsInfo *errorsInfo);

    protected:
        virtual void highlightBlock(const QString &text) override;

    private:
        SpellCheckErrorsInfo *m_SpellCheckErrors;
        QMLExtensions::ColorsModel *m_ColorsModel;
    };
}

#endif // SPELLCHECKERRORSHIGHLIGHTER_H
