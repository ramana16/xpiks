/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef STRINGHELPER
#define STRINGHELPER

#include <string>
#include <vector>
#include <QString>
#include <QVector>
#include <stdarg.h>
#include <functional>

class QString;
class QStringList;
class QByteArray;

namespace Helpers {
    void foreachPart(const QString &text,
                     const std::function<bool (const QChar &symbol)> &isSeparatorPred,
                     const std::function<bool (const QString &word)> &pred,
                     const std::function<void (int start, int length, const QString &word)> &action);
    void foreachWord(const QString &text,
                     const std::function<bool (const QString &word)> &pred,
                     const std::function<void (int start, int length, const QString &word)> &action);
    QString replaceWholeWords(const QString &text, const QString &replaceWhat,
                              const QString &replaceTo, Qt::CaseSensitivity caseSensitivity=Qt::CaseInsensitive);
    bool containsWholeWords(const QString &haystack, const QString &needle, Qt::CaseSensitivity caseSensitivity=Qt::CaseInsensitive);
    QString getLastNLines(const QString &text, int N);
    void splitText(const QString &text, QStringList &parts);
    void splitKeywords(const QString &text, const QVector<QChar> &separators, QStringList &parts);
    int levensteinDistance(const QString &s1, const QString &s2);
    bool is7BitAscii(const QByteArray &s);
    bool isPunctuation(const QChar &);
    std::string string_format(const std::string fmt, ...);
    QString getUnitedHitsString(const QString &text, const std::vector<int> &hits, int radius);

    quint32 switcherHash(const QString &text);
}

#endif // STRINGHELPER

