/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "jsonhelper.h"

namespace Helpers {
    bool mergeJsonArrays (const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer);
    bool mergeJsonObjects (const QJsonObject &objectMergeFrom, QJsonObject &objectMergeTo, CompareValuesJson &comparer);

    bool mergeArraysOfObjects(const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer);
    int findObjectIndex(const QJsonArray &array, int minIndex, const QJsonObject &object, CompareValuesJson &comparer);
    bool mergeArraysOfStrings(const QJsonArray &arrayFrom, QJsonArray &arrayTo);

    bool mergeJsonArrays (const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        if (arrayTo.isEmpty()) {
            LOG_DEBUG << "ArrayTo is empty";
            arrayTo = arrayFrom;
            return true;
        }

        if (arrayFrom.isEmpty()) {
            LOG_DEBUG << "ArrayFrom is empty";
            return false;
        }

        if (arrayTo.first().type() != arrayFrom.first().type() ) {
            LOG_WARNING << "Type of arrays to be merged does not match!";
            return false;
        }

        bool mergeResult = false;
        const int type = arrayTo.first().type();

        if (type == QJsonValue::Object) {
            mergeResult = mergeArraysOfObjects(arrayFrom, arrayTo, comparer);
        } else if (type == QJsonValue::String) {
            mergeResult = mergeArraysOfStrings(arrayFrom, arrayTo);
        } else {
            LOG_WARNING << "Unsupported type of QJsonArray:" << type;
        }

        return mergeResult;
    }

    bool mergeArraysOfObjects(const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        int minSize = qMin(arrayTo.size(), arrayFrom.size());

        int i = 0;
        // skip same elements
        while (i < minSize) {
            QJsonObject objectTo = arrayTo[i].toObject();
            QJsonObject objectFrom = arrayFrom[i].toObject();

            if (comparer(objectTo, objectFrom) == 0) {
                if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                    arrayTo[i] = objectTo;
                }

                i++;
            } else {
                break;
            }
        }

        LOG_DEBUG << i << "same elements skipped";

        int maxIndexOfEqual = i;

        const int sizeFrom = arrayFrom.size();
        QJsonArray elementsToAdd;

        for (; i < sizeFrom; i++) {
            Q_ASSERT(arrayFrom[i].isObject());
            QJsonObject objectFrom = arrayFrom[i].toObject();

            const int existingIndex = findObjectIndex(arrayTo, maxIndexOfEqual, objectFrom, comparer);
            if (existingIndex == -1) {
                elementsToAdd.append(objectFrom);
            } else {
                QJsonObject objectTo = arrayTo[i].toObject();

                if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                    arrayTo[i] = objectTo;
                }
            }
        }

        LOG_DEBUG << elementsToAdd.size() << "new elements to be added";

        QJsonArray mergedArray = arrayTo;
        int sizeMergedAdd = elementsToAdd.size();
        for (int t = 0; t < sizeMergedAdd; t++) {
            mergedArray.append(elementsToAdd[t]);
        }

        arrayTo = mergedArray;

        return true;
    }

    int findObjectIndex(const QJsonArray &array, int minIndex, const QJsonObject &object, CompareValuesJson &comparer) {
        int index = -1;
        Q_ASSERT(minIndex < array.size());

        const int size = array.size();

        for (int i = minIndex; i < size; ++i) {
            Q_ASSERT(array[i].type() == QJsonValue::Object);

            QJsonObject objectInArray = array[i].toObject();

            if (comparer(object, objectInArray) == 0) {
                index = i;
                break;
            }
        }

        return index;
    }

    bool mergeArraysOfStrings(const QJsonArray &arrayFrom, QJsonArray &arrayTo) {
        QJsonArray arrayMerged;

        QSet<QString> commonValues;
        commonValues.reserve(arrayTo.size() + arrayFrom.size());

        int i = 0;
        const int sizeTo = arrayTo.size();
        for (i = 0; i < sizeTo; ++i) {
            Q_ASSERT(arrayTo[i].type() == QJsonValue::String);
            commonValues.insert(arrayTo[i].toString());
        }

        const int sizeFrom = arrayFrom.size();
        for (i = 0; i < sizeFrom; ++i) {
            Q_ASSERT(arrayFrom[i].type() == QJsonValue::String);
            commonValues.insert(arrayFrom[i].toString());
        }

        QSet<QString>::iterator begin = commonValues.begin();
        QSet<QString>::iterator end = commonValues.end();

        for (QSet<QString>::iterator it = begin; it != end; ++it) {
            arrayMerged.append(*it);
        }

        arrayTo = arrayMerged;
        return true;
    }

    bool mergeJsonObjects(const QJsonObject &objectMergeFrom, QJsonObject &objectMergeTo, CompareValuesJson &comparer) {
        LOG_INTEGR_TESTS_OR_DEBUG << "#";

        QStringList keysMergeFrom = objectMergeFrom.keys();
        int keysSize = keysMergeFrom.size();

        bool anyError = false;

        for (int i = 0; i < keysSize; i++) {
            const QString &keyFrom = keysMergeFrom.at(i);

            if (objectMergeTo.contains(keyFrom)) {
                QJsonValue valueTo = objectMergeTo[keyFrom];
                QJsonValue valueFrom = objectMergeFrom[keyFrom];

                if (valueTo.type() != valueFrom.type()) {
                    LOG_WARNING << "Types of Json Values do not match at key:" << keyFrom;
                    continue;
                }

                if (valueTo.isObject()) {
                    QJsonObject objectTo = valueTo.toObject();
                    QJsonObject objectFrom = valueFrom.toObject();

                    if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                        valueTo = objectTo;
                    } else {
                        anyError = true;
                        break;
                    }
                } else if (valueTo.isArray()) {
                    QJsonArray arrayTo = valueTo.toArray();
                    QJsonArray arrayFrom = valueFrom.toArray();

                    if (mergeJsonArrays(arrayFrom, arrayTo, comparer)) {
                        valueTo = arrayTo;
                    } else {
                        anyError = true;
                        break;
                    }
                } else {
                    // overwrite if found plain type
                    valueTo = valueFrom;
                }

                objectMergeTo[keyFrom] = valueTo;

            } else {
                // insert if not found
                objectMergeTo[keyFrom] = objectMergeFrom[keyFrom];
            }
        }

        bool mergeResult = !anyError;
        return mergeResult;
    }

    void mergeJson(const QJsonDocument &mergeFrom, QJsonDocument &mergeTo, bool overwrite, CompareValuesJson &comparer) {
        if (mergeFrom.isNull()) {
            LOG_WARNING << "attempted to merge with Null json document";
            return;
        }

        if (overwrite || mergeTo.isNull()) {
            LOG_DEBUG << "Overwriting the document";
            mergeTo = mergeFrom;
        } else {
            if (mergeFrom.isArray()) {
                QJsonArray arrayFrom = mergeFrom.array();
                QJsonArray arrayTo = mergeTo.array();

                if (mergeJsonArrays(arrayFrom, arrayTo, comparer)) {
                    mergeTo.setArray(arrayTo);
                }
            } else {
                QJsonObject objectFrom = mergeFrom.object();
                QJsonObject objectTo = mergeTo.object();

                if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                    mergeTo.setObject(objectTo);
                }
            }
        }
    }
}
