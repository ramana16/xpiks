/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAHIGHLIGHTER_H
#define METADATAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>
#include "../Common/iflagsprovider.h"

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class MetadataHighlighter:
        public QSyntaxHighlighter
    {
    public:
        MetadataHighlighter(const QString &textToHighlight,
                            Common::IFlagsProvider *flagsProvider,
                            QMLExtensions::ColorsModel *colorsModel,
                            QTextDocument *document=0);

    protected:
        void highlightBlock(const QString &text);

    private:
        QMLExtensions::ColorsModel *m_ColorsModel;
        QString m_TextToHighlight;
        Common::IFlagsProvider *m_FlagsProvider;
        QTextCharFormat m_Format;
    };
}

#endif // METADATAHIGHLIGHTER_H
