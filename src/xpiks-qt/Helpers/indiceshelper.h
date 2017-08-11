/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INDICESHELPER_H
#define INDICESHELPER_H

#include <QVector>
#include <QPair>
#include <vector>
#include <utility>

namespace Helpers {
    typedef std::vector<std::pair<int, int> > RangesVector;

    template<class T>
    int splitIntoChunks(const QVector<T> &items, int chunksCount, QVector<QVector<T> > &chunks);

    void indicesToRanges(const QVector<int> &indices, QVector<QPair<int, int> > &ranges);
    int getRangesLength(const QVector<QPair<int, int> > &ranges);
    RangesVector unionRanges(RangesVector &ranges);
}

#endif // INDICESHELPER_H
