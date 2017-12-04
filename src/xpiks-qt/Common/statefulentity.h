/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef STATEFULENTITY_H
#define STATEFULENTITY_H

#include "../Helpers/localconfig.h"
#include <QString>
#include <QJsonObject>

namespace Common {
    class StatefulEntity
    {
    public:
        StatefulEntity(const QString &stateName);

    protected:
        void initState();
        void syncState();

    protected:
        inline void setStateValue(const char *key, const QJsonValue &value) {
            m_StateJson.insert(QLatin1String(key), value);
        }

        inline QJsonValue getStateValue(const char *key, const QJsonValue &defaultValue = QJsonValue()) const {
            QJsonValue value = m_StateJson.value(QLatin1String(key));

            if (value.isUndefined()) {
                return defaultValue;
            }

            return value;
        }

        inline bool getStateBool(const char *key, const bool defaultValue = false) const {
            return m_StateJson.value(QLatin1String(key)).toBool(defaultValue);
        }

        inline double getStateDouble(const char *key, const double defaultValue = 0) const {
            return m_StateJson.value(QLatin1String(key)).toDouble(defaultValue);
        }

        inline int getStateInt(const char *key, const int defaultValue = 0) const {
            return m_StateJson.value(QLatin1String(key)).toInt(defaultValue);
        }

        inline QString getStateString(const char *key, const QString &defaultValue = QString("")) const {
            return m_StateJson.value(QLatin1String(key)).toString(defaultValue);
        }

        inline bool containsState(const char *key) const {
            return m_StateJson.contains(QLatin1String(key));
        }

    private:
        QString m_StateName;
        Helpers::LocalConfig m_StateConfig;
        QJsonObject m_StateJson;
    };
}

#endif // STATEFULENTITY_H
