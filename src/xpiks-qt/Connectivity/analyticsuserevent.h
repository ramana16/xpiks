/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ANALYTICSUSEREVENT
#define ANALYTICSUSEREVENT

#include <QDateTime>
#include <QString>

namespace Connectivity {
    enum struct UserAction {
        Open,
        Close,
        Upload,
        SpellCheck,
        SpellSuggestions,
        SuggestionRemote,
        SuggestionLocal,
        WarningsCheck,
        TurnOffTelemetry,
        FindAndReplace,
        DeleteKeywords
    };

    class AnalyticsUserEvent {
    public:
        AnalyticsUserEvent(UserAction action) :
            m_Action(action)
        {
            m_DateTime = QDateTime::currentDateTimeUtc();
        }

        QString getActionString() const {
            switch (m_Action) {
            case UserAction::Open: return QLatin1String("Open");
            case UserAction::Close: return QLatin1String("Close");
            case UserAction::Upload: return QLatin1String("Upload");
            case UserAction::SpellCheck: return QLatin1String("SpellCheck");
            case UserAction::SpellSuggestions: return QLatin1String("SpellSuggestions");
            case UserAction::SuggestionRemote: return QLatin1String("SuggestionRemote");
            case UserAction::SuggestionLocal: return QLatin1String("SuggestionLocal");
            case UserAction::WarningsCheck: return QLatin1String("WarningsCheck");
            case UserAction::TurnOffTelemetry: return QLatin1String("TurnOffTelemetry");
            case UserAction::FindAndReplace: return QLatin1String("UserActionFindAndReplace");
            case UserAction::DeleteKeywords: return QLatin1String("UserActionDeleteKeywords");
            default: return QLatin1String("Unknown");
            }
        }

        int getHour() const {
            return m_DateTime.time().hour();
        }

        int getMinute() const {
            return m_DateTime.time().minute();
        }

        int getSecond() const {
            return m_DateTime.time().second();
        }

    private:
        UserAction m_Action;
        QDateTime m_DateTime;
    };

}

#endif // ANALYTICSUSEREVENT

