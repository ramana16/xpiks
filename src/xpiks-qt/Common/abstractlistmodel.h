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
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
            int rangesLength = Helpers::getRangesLength(ranges);
            LOG_INFO << "Ranges length:" << rangesLength;
            doRemoveItemsAtIndices(ranges, rangesLength);
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

        void doRemoveItemsAtIndices(const QVector<QPair<int, int> > &ranges, int rangesLength) {
            const int rangesCount = ranges.count();
            const int rangesLengthForReset = getRangesLengthForReset();

            const bool willResetModel = rangesLength >= rangesLengthForReset;

            if (willResetModel) {
                beginResetModel();
            }
            
            QModelIndex dummy;

            for (int i = rangesCount - 1; i >= 0; --i) {
                const int startRow = ranges[i].first;
                const int endRow = ranges[i].second;

                if (!willResetModel) {
                    beginRemoveRows(dummy, startRow, endRow);
                }

                removeInnerItemRange(startRow, endRow);

                if (!willResetModel) {
                    endRemoveRows();
                }
            }

            if (willResetModel) {
                endResetModel();
            }
        }
    };
}
#endif // ABSTRACTLISTMODEL

