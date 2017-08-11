/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
