/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTOCOMPLETEMODEL_H
#define AUTOCOMPLETEMODEL_H

#include <QAbstractListModel>
#include <QMutex>
#include <QVector>
#include <QString>
#include <QSet>
#include <vector>
#include <memory>
#include "../Common/baseentity.h"
#include "completionitem.h"

namespace AutoComplete {
    class AutoCompleteModel : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
        Q_PROPERTY(bool isActive READ getIsActive WRITE setIsActive NOTIFY isActiveChanged)
    public:
        AutoCompleteModel(QObject *parent=0);

    public:
        enum AutoCompleteModelRoles {
            IsPresetRole = Qt::UserRole + 1,
        };

    public:
        void setCompletions(const QStringList &completions);
        void setPresetsMembership(const QSet<QString> &presetsMembership);

    public:
        int getSelectedIndex() const { return m_SelectedIndex; }
        bool getIsActive() const { return m_IsActive; }

    public:
        void setSelectedIndex(int value);

        void setIsActive(bool value) {
            if (value != m_IsActive) {
                m_IsActive = value;
                emit isActiveChanged(value);
            }
        }

    public:
        Q_INVOKABLE void sync();
        Q_INVOKABLE bool moveSelectionUp();
        Q_INVOKABLE bool moveSelectionDown();
        Q_INVOKABLE void cancelCompletion() { setIsActive(false); emit dismissPopupRequested(); }
        Q_INVOKABLE void acceptSelected(bool tryExpandPreset=false);
        Q_INVOKABLE int getCount() const { return (int)m_CompletionList.size(); }
        Q_INVOKABLE bool hasSelectedCompletion() const { return (0 <= m_SelectedIndex) && (m_SelectedIndex < rowCount()); }

    signals:
        void selectedIndexChanged(int index);
        void dismissPopupRequested();
        void completionAccepted(const QString &completion, bool expandPreset);
        void isActiveChanged(bool value);
        void completionsAvailable();

    public slots:
        void onUpdatesArrived();

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override { Q_UNUSED(parent); return (int)m_CompletionList.size(); }
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

#ifdef INTEGRATION_TESTS
        bool containsWord(const QString &word) const;
        QStringList getLastGeneratedCompletions();
#endif

    private:
        QMutex m_Mutex;
        std::vector<std::shared_ptr<CompletionItem> > m_CompletionList;
        std::vector<std::shared_ptr<CompletionItem> > m_LastGeneratedCompletions;
        QSet<QString> m_PresetsMembership;
        int m_SelectedIndex;
        bool m_IsActive;
    };
}

#endif // AUTOCOMPLETEMODEL_H
