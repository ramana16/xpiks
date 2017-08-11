/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "keywordshelpers.h"
#include <QRegularExpression>

#define ALLOWED_SYMBOLS "@#%&!?"

namespace Helpers {
    bool isValidKeyword(const QString &keyword) {
        bool isValid = false;
        int length = keyword.length();

        if (2 <= length && length < 30) {
            isValid = true;
        }

        if (length == 1) {
            QChar symbol = keyword.at(0);
            if (symbol.isLetterOrNumber() ||
                    QString(ALLOWED_SYMBOLS).contains(symbol) ||
                    symbol.category() == QChar::Symbol_Currency) {
                isValid = true;
            }
        }

        return isValid;
    }

    QString doSanitizeKeyword(const QString &keyword) {
        QString allowed = QString(ALLOWED_SYMBOLS);
        int start = 0;
        int length = keyword.length();
        QChar c;
        while (start < length) {
            c = keyword.at(start);
            if (c.isLetterOrNumber() ||
                    allowed.contains(c) ||
                    c.category() == QChar::Symbol_Currency) {
                break;
            } else {
                start++;
            }
        }

        int end = length - 1;
        while (end >= 0) {
            c = keyword.at(end);
            if (c.isLetterOrNumber() ||
                    allowed.contains(c) ||
                    c.category() == QChar::Symbol_Currency) {
                break;
            } else {
                end--;
            }
        }

        QString result;
        if (start <= end) {
            result = keyword.mid(start, end - start + 1).simplified();
        }

        return result;
    }
}
