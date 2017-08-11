/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class LogHighlighter: public QSyntaxHighlighter {
        Q_OBJECT
    public:
        LogHighlighter(QMLExtensions::ColorsModel *colorsModel, QTextDocument* document = 0);

    protected:
        void highlightBlock(const QString &text);

    private:
        QMLExtensions::ColorsModel *m_ColorsModel;
    };
}
#endif // LOGHIGHLIGHTER_H
