/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

        virtual bool shouldRemoveInRanges(int rangesLength) const { return rangesLength > RANGES_LENGTH_FOR_RESET; }

        virtual void removeInnerItemRange(int startRow, int endRow) {
            for (int row = endRow; row >= startRow; --row) { removeInnerItem(row); }
        }

        void doRemoveItemsAtIndices(const QVector<QPair<int, int> > &ranges, int rangesLength) {
            const int rangesCount = ranges.count();

            const bool willResetModel = shouldRemoveInRanges(rangesLength);

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

