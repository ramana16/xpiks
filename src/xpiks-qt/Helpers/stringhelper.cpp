/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "stringhelper.h"
#include <QFile>
#include <QTextStream>
#include <QStringRef>
#include <QVector>
#include <QByteArray>
#include <QCryptographicHash>
#include <QString>
#include <QtGlobal>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"

#if defined(Q_OS_WIN)
#define NOMINMAX
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#elif defined(Q_OS_MAC)
 #include <arpa/inet.h>
#elif defined(Q_OS_LINUX)
#include <arpa/inet.h>
#endif

#define SYNONYMS_DISTANCE 3

namespace Helpers {
    void foreachPart(const QString &text,
                     const std::function<bool (const QChar &symbol)> &isSeparatorPred,
                     const std::function<bool (const QString &word)> &pred,
                     const std::function<void (int start, int length, const QString &word)> &action)
    {
        int i = 0;
        const int size = text.size();
        int lastStart = -1;

        while (i < size) {
            QChar c = text[i];
            if (isSeparatorPred(c)) {
                if (lastStart != -1) {
                    int wordLength = i - lastStart;
                    QString word = text.mid(lastStart, wordLength);

                    if (pred(word)) {
                        action(lastStart, wordLength, word);
                    }

                    lastStart = -1;
                }
            } else {
                if (lastStart == -1) {
                    lastStart = i;
                }
            }

            i++;
        }

        if (lastStart != -1) {
            int wordLength = size - lastStart;
            QString word = text.mid(lastStart, wordLength);

            if (pred(word)) {
                action(lastStart, wordLength, word);
            }
        }
    }

    void foreachWord(const QString &text,
            const std::function<bool (const QString &word)> &pred,
            const std::function<void (int start, int length, const QString &word)> &action)
    {
        foreachPart(text,
                    [](const QChar &c) { return c.isSpace() || isPunctuation(c); },
        pred, action);
    }

    bool isLeftWordBound(const QString &text, int index, bool skipWordBounds=false) {
        if (index == 0) { return true; }

        QChar curr = text[index];
        QChar prev = text[index - 1];

        const bool currIsSeparator = isPunctuation(curr) || curr.isSpace();
        const bool prevIsSeparator = isPunctuation(prev) || prev.isSpace();

        return (skipWordBounds || !currIsSeparator) && (prevIsSeparator);
    }

    bool isRightWordBound(const QString &text, int lastIndex, int index, bool skipWordBounds=false) {
        if (index == lastIndex) { return true; }

        QChar curr = text[index];
        QChar next = text[index + 1];

        const bool currIsSeparator = isPunctuation(curr) || curr.isSpace();
        const bool nextIsSeparator = isPunctuation(next) || next.isSpace();

        return (skipWordBounds || !currIsSeparator) && (nextIsSeparator);
    }

    bool isAWholeWord(const QString &text, int start, int length, bool onlyCheckBounds=false) {
        if (text.isEmpty()) { return false; }

        if (!isLeftWordBound(text, start, onlyCheckBounds)) { return false; }
        const int lastIndex = text.size() - 1;
        if (!isRightWordBound(text, lastIndex, start + length - 1, onlyCheckBounds)) { return false; }

        return true;
    }

    QString replaceWholeWords(const QString &text, const QString &replaceWhat,
                              const QString &replaceTo, Qt::CaseSensitivity caseSensitivity) {
        int pos = 0;
        const int size = replaceWhat.size();
        std::vector<std::pair<int, int> > hits;
        hits.reserve(std::max(text.length() / replaceWhat.length(), 10));

        while (pos != -1) {
            pos = text.indexOf(replaceWhat, pos, caseSensitivity);
            if (pos >= 0) {
                if (isAWholeWord(text, pos, size, true)) {
                    hits.emplace_back(std::make_pair(pos, size));
                }

                pos += size;
            }
        }

        if (hits.empty()) { return text; }

        QString result;
        result.reserve(text.length());

        int lastStart = 0;
        for (auto &hit: hits) {
            if (hit.first > lastStart) {
                result.append(text.mid(lastStart, hit.first - lastStart));
            }

            result.append(replaceTo);
            lastStart = hit.first + hit.second;
        }

        if (lastStart < text.length() - 1) {
            result.append(text.mid(lastStart));
        }

        return result;
    }

    bool containsWholeWords(const QString &haystack, const QString &needle, Qt::CaseSensitivity caseSensitivity) {
        bool anyHit = false;

        int pos = 0;
        const int size = needle.size();

        while (pos != -1) {
            pos = haystack.indexOf(needle, pos, caseSensitivity);
            if (pos >= 0) {
                if (isAWholeWord(haystack, pos, size, true)) {
                    anyHit = true;
                    break;
                }

                pos += size;
            }
        }

        return anyHit;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString getLastNLines(const QString &text, int N) {
        QString result;

        QVector<QStringRef> items = text.splitRef(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        const int length = items.length();

        if (length > 0) {
            int startIndex = length - N;
            if (startIndex < 0) {
                startIndex = 0;
            }

            int pos = items[startIndex].position();
            result = text.right(text.length() - pos);
        } else {
            result = text;
        }

        return result;
    }

#else
    QString getLastNLines(const QString &text, int N) {
        QString result;
        QStringList lastNLines;

        QStringList items = text.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        int length = items.length();

        if (length > 0) {
            int startIndex = length - N;
            if (startIndex < 0) {
                startIndex = 0;
            }

            for (int pos = startIndex; pos < length; pos++) {
                lastNLines.append(items[pos]);
            }

            result = lastNLines.join("\n");
        } else {
            result = text;
        }

        return result;
    }

#endif

    void splitText(const QString &text, QStringList &parts) {
        foreachWord(text,
                    [](const QString&) { return true; },
        [&parts](int, int, const QString &word) { parts.append(word); });
    }

    void splitKeywords(const QString &text, const QVector<QChar> &separators, QStringList &parts) {
        foreachPart(text,
                    [&separators](const QChar &c) { return separators.contains(c); },
        [](const QString&) { return true; },
        [&parts](int, int, const QString &word) { parts.append(word); });
    }

    std::string string_format(const std::string fmt, ...) {
        int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
        std::string str;
        va_list ap;
        while (1) {     // Maximum two passes on a POSIX system...
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
            va_end(ap);
            if (n > -1 && n < size) {  // Everything worked
                str.resize(n);
                return str;
            }
            if (n > -1)  // Needed size returned
                size = n + 1;   // For null char
            else
                size *= 2;      // Guess at a larger size (OS specific)
        }

        return str;
    }

    unsigned int levensteinDistance(const QString &a, const QString &b) {
        const unsigned int lengthA = a.size(), lengthB = b.size();
        std::vector<unsigned int> costs(lengthB + 1), prevCosts(lengthB + 1);
        const unsigned int prevCostsSize = (unsigned int)prevCosts.size();

        for (unsigned int i = 0; i < prevCostsSize; i++) {
            prevCosts[i] = i;
        }

        for (unsigned int i = 0; i < lengthA; i++) {
            costs[0] = i + 1;

            for (unsigned int j = 0; j < lengthB; j++) {
                costs[j + 1] = std::min(
                    std::min(prevCosts[1 + j] + 1, costs[j] + 1),
                    prevCosts[j] + (a[i] == b[j] ? 0 : 1));
            }

            costs.swap(prevCosts);
        }

        unsigned int result = prevCosts[lengthB];
        return result;
    }

    int levensteinPercentage(const QString &s1, const QString &s2) {
        int maxLength = std::max(s1.length(), s2.length());

        unsigned int distance = levensteinDistance(s1.toLower(), s2.toLower());
        int reverseDistance = maxLength - (int)distance;

        if (reverseDistance == 0) { return 0; }

        int percent = (reverseDistance * 100) / maxLength;
        return percent;
    }

    bool is7BitAscii(const QByteArray &s) {
        bool anyFault = false;
        const int size = s.size();

        for (int i = 0; i < size; i++) {
            int c = s[i];
            if (c < 0 || c >= 128) {
                anyFault = true;
                break;
            }
        }

        return !anyFault;
    }

    bool isPunctuation(const QChar &c) {
        return c.isPunct() && c != QChar('/');
    }

    void extendSegmentToWordBoundaries(const QString &text, std::pair<int, int> &segment) {
        int left = segment.first;
        while (!isLeftWordBound(text, left)) { left--; }

        int right = segment.second;
        const int lastIndex = text.size() - 1;
        while (!isRightWordBound(text, lastIndex, right)) { right++; }

        segment.first = left;
        segment.second = right;
    }

    std::pair<int, int> getSegmentFromHit(const QString &text, int hit, int radius) {
        int left = hit;
        while (!isLeftWordBound(text, left)) { left--; }

        int right = hit;
        const int lastIndex = text.size() - 1;
        while (!isRightWordBound(text, lastIndex, right)) { right++; }

        int first = std::max(0, left - radius);
        int second = std::min(lastIndex, right + radius);
        return std::make_pair(first, second);
    }

    QString getUnitedHitsString(const QString &text, const std::vector<int> &hits, int radius) {
        std::vector<std::pair<int, int> > segments;
        segments.resize(hits.size());

        // create segment from each hit
        std::transform(hits.begin(), hits.end(), segments.begin(),
                       [&text, radius](int hit) {
            return getSegmentFromHit(text, hit, radius);
        });

        for (auto &item: segments) {
            extendSegmentToWordBoundaries(text, item);
        }

        auto unitedSegments = Helpers::unionRanges(segments);

        QStringList entries;
        entries.reserve((int)unitedSegments.size());

        for (auto &element: unitedSegments) {
            entries.append(text.mid(element.first, element.second - element.first + 1));
        }

        QString result = entries.join(" ... ");
        return result;
    }

    quint32 switcherHash(const QString &text) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(text.toUtf8());
        QByteArray result = hash.result();

        char *data = result.data();
        quint32 prefix = ntohl(*((quint32 *)data));
        return prefix;
    }

    bool areSemanticDuplicates(const QString &s1, const QString &s2) {
        if (QString::compare(s1, s2, Qt::CaseInsensitive) == 0) { return true; }

        const int length1 = s1.length();
        const int length2 = s2.length();

        const int diff = abs(length1 - length2);
        if (diff > SYNONYMS_DISTANCE) { return false; }

        const auto distance = Helpers::levensteinDistance(s1.toLower(), s2.toLower());
        return distance <= SYNONYMS_DISTANCE;
    }
}
