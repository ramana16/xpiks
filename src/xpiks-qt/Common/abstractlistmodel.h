/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ABSTRACTLISTMODEL
#define ABSTRACTLISTMODEL

#include <QAbstractListModel>

#include <QVector>
#include <QList>
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"

#define RANGES_LENGTH_FOR_RESET 40

namespace Common {
    class AbstractListModel : public QAbstractListModel {
        Q_OBJECT
    public:
        AbstractListModel(QObject *parent = 0) : QAbstractListModel(parent) {}
        virtual ~AbstractListModel() {}

    public:
        virtual void removeItemsFromRanges(const QVector<QPair<int, int> > &ranges) {
            doRemoveItemsFromRanges(ranges);
            emitRemovedSignalsForRanges(ranges);
        }

        virtual void emitRemovedSignalsForRanges(const QVector<QPair<int, int> > &ranges) {
            if (ranges.empty()) { return; }

            int rangesLength = Helpers::getRangesLength(ranges);
            LOG_INFO << "Ranges length:" << rangesLength;
            doEmitItemsRemovedAtIndices(ranges, rangesLength);
        }

    protected:
        void updateItemsInRanges(const QVector<QPair<int, int> > &ranges, const QVector<int> &roles) {
            int rangesCount = ranges.count();
            for (int i = 0; i < rangesCount; ++i) {
                int startRow = ranges[i].first;
                int endRow = ranges[i].second;

                QModelIndex topLeft = index(startRow);
                QModelIndex bottomRight = index(endRow);
                emit dataChanged(topLeft, bottomRight, roles);
            }
        }

        virtual void removeInnerItem(int row) = 0;

        virtual int getRangesLengthForReset() const { return RANGES_LENGTH_FOR_RESET; }

        virtual void removeInnerItemRange(int startRow, int endRow) {
            for (int row = endRow; row >= startRow; --row) { removeInnerItem(row); }
        }

        void doRemoveItemsFromRanges(const QVector<QPair<int, int> > &ranges) {
            const int rangesCount = ranges.count();
            for (int i = rangesCount - 1; i >= 0; --i) {
                const int startRow = ranges[i].first;
                const int endRow = ranges[i].second;

                removeInnerItemRange(startRow, endRow);
            }
        }

        void doEmitItemsRemovedAtIndices(const QVector<QPair<int, int> > &ranges, int rangesLength) {
            const int rangesLengthForReset = getRangesLengthForReset();
            const bool willResetModel = rangesLength >= rangesLengthForReset;

            if (willResetModel) {
                beginResetModel();
                endResetModel();
            } else {
                const int rangesCount = ranges.count();
                QModelIndex dummy;

                for (int i = rangesCount - 1; i >= 0; --i) {
                    const int startRow = ranges[i].first;
                    const int endRow = ranges[i].second;

                    beginRemoveRows(dummy, startRow, endRow);
                    endRemoveRows();
                }
            }
        }
    };
}
#endif // ABSTRACTLISTMODEL

