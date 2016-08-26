/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANALYTICSUSEREVENT
#define ANALYTICSUSEREVENT

#include <QDateTime>
#include <QString>

namespace Conectivity {
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

