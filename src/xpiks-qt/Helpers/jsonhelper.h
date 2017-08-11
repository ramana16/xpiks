/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QSet>
#include "../Common/defines.h"
#include "comparevaluesjson.h"

namespace Helpers {
    void mergeJson(const QJsonDocument &mergeFrom, QJsonDocument &mergeTo,
                   bool overwrite, CompareValuesJson &comparer);
}

#endif // JSONHELPER_H
