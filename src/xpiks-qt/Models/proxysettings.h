/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include "../Common/defines.h"

namespace Models {
    class ProxySettings {
    public:
        QString m_Address;
        QString m_User;
        QString m_Password;
        QString m_Port;
    };

    QDataStream &operator<<(QDataStream &out, const ProxySettings &v);
    QDataStream &operator>>(QDataStream &in, ProxySettings &v);
}

Q_DECLARE_METATYPE(Models::ProxySettings)

#endif // PROXYSETTINGS_H
