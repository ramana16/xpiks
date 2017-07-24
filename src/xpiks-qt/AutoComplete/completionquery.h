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

#ifndef COMPLETIONQUERY_H
#define COMPLETIONQUERY_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include "autocompletemodel.h"
#include "completionitem.h"
#include "../Common/defines.h"

namespace AutoComplete {
    class CompletionQuery : public QObject {
        Q_OBJECT
    public:
        CompletionQuery(const QString &prefix, AutoCompleteModel *autoCompleteModel) :
            m_Prefix(prefix),
            m_AutoCompleteModel(autoCompleteModel),
            m_NeedsUpdate(false)
        {
            Q_ASSERT(autoCompleteModel != nullptr);
        }

        const QString &getPrefix() const { return m_Prefix; }
        const QStringList &getCompletions() const { return m_Completions; }

        void setCompletions(const QStringList &completions) {
            m_Completions = completions;
            m_AutoCompleteModel->setCompletions(completions);
            emit completionsAvailable();
        }

        void propagateUpdates() {
            m_AutoCompleteModel->setPresetsMembership(m_PresetsSet);
            emit updatesAvailable();
        }

        void setNeedsUpdate() { m_NeedsUpdate = true; }
        bool getNeedsUpdate() const { return m_NeedsUpdate; }

        void setIsPreset(const QString &completion) {
            m_PresetsSet.insert(completion);
        }

        bool getIsPreset(const QString &completion) {
            bool isPreset = m_PresetsSet.contains(completion);
            return isPreset;
        }

    signals:
        void completionsAvailable();
        void updatesAvailable();

    private:
        QString m_Prefix;
        QStringList m_Completions;
        QSet<QString> m_PresetsSet;
        AutoCompleteModel *m_AutoCompleteModel;
        volatile bool m_NeedsUpdate;
    };
}

#endif // COMPLETIONQUERY_H
