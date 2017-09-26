/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DUPLICATESHIGHLIGHTER_H
#define DUPLICATESHIGHLIGHTER_H

#include <QtGui>
#include <QSet>
#include <QString>

namespace QMLExtensions {
    class ColorsModel;
}

namespace SpellCheck {
    class SpellCheckErrorsInfo;

    class DuplicatesHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT
    public:
        DuplicatesHighlighter(QTextDocument *document,
                              QMLExtensions::ColorsModel *colorsModel,
                              SpellCheckErrorsInfo *errorsInfo);
        virtual ~DuplicatesHighlighter();

    protected:
        virtual void highlightBlock(const QString &text) override;

    public slots:
        void keywordsDuplicatesChanged();

    private:
        SpellCheckErrorsInfo *m_SpellCheckErrors;
        QMLExtensions::ColorsModel *m_ColorsModel;
        bool m_HighlighAll;
    };
}

#endif // DUPLICATESHIGHLIGHTER_H
