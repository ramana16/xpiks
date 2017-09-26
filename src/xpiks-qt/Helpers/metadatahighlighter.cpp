/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadatahighlighter.h"
#include "../QMLExtensions/colorsmodel.h"
#include "../Helpers/stringhelper.h"
#include "../Common/flags.h"

namespace Helpers {
    MetadataHighlighter::MetadataHighlighter(const QString &textToHighlight,
                                             Common::IFlagsProvider *flagsProvider,
                                             QMLExtensions::ColorsModel *colorsModel,
                                             QTextDocument *document):
        QSyntaxHighlighter(document),
        m_ColorsModel(colorsModel),
        m_TextToHighlight(textToHighlight),
        m_FlagsProvider(flagsProvider)
    {
        Q_ASSERT(!textToHighlight.isEmpty());
        Q_ASSERT(flagsProvider != nullptr);
        Q_ASSERT(colorsModel != nullptr);
    }

    void MetadataHighlighter::highlightBlock(const QString &text) {
        if (text.isEmpty()) { return; }

        int pos = 0;
        const int size = m_TextToHighlight.size();
        Common::flag_t flags = m_FlagsProvider->getFlags();
        Qt::CaseSensitivity caseSensitivity = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive) ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::WholeWords);
        const bool searchOnlySpaces = m_TextToHighlight.trimmed().isEmpty();

        QColor highlighColor = m_ColorsModel->artworkModifiedColor();
        m_Format.setBackground(highlighColor);

        if (!wholeWords || searchOnlySpaces) {
            while (pos != -1) {
                pos = text.indexOf(m_TextToHighlight, pos, caseSensitivity);
                if (pos >= 0) {
                    setFormat(pos, size, m_Format);
                    pos += size;
                }
            }
        } else {
            Helpers::foreachWord(text,
                                 [this, &caseSensitivity](const QString &word) {
                return (QString::compare(this->m_TextToHighlight, word, caseSensitivity) == 0);
            },
            [this](int start, int length, const QString&) {
                this->setFormat(start, length, this->m_Format);
            });
        }
    }
}
