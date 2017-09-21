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

#include <QObject>
#include <QAbstractListModel>
#include "../Common/baseentity.h"
#include "completionitem.h"

namespace AutoComplete {
    class AutoCompleteModel : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
        Q_PROPERTY(bool isActive READ getIsActive WRITE setIsActive NOTIFY isActiveChanged)
    public:
        explicit AutoCompleteModel(QObject *parent = 0);

    public:
        int getSelectedIndex() const { return m_SelectedIndex; }
        bool getIsActive() const { return m_IsActive; }

    public:
        void setSelectedIndex(int value);
        void setIsActive(bool value);
        void clear() { clearCompletions(); }

    public:
        Q_INVOKABLE void initializeCompletions();
        Q_INVOKABLE bool moveSelectionUp();
        Q_INVOKABLE bool moveSelectionDown();
        Q_INVOKABLE void cancelCompletion();
        Q_INVOKABLE void acceptSelected(bool withMetaAction=false);
        Q_INVOKABLE int getCount() { return getCompletionsCount(); }
        Q_INVOKABLE bool hasSelectedCompletion() { return (0 <= m_SelectedIndex) && (m_SelectedIndex < getCompletionsCount()); }
        Q_INVOKABLE QString getCompletion(int completionID) { return doGetCompletion(completionID); }
        Q_INVOKABLE QObject *getCompletionsModel();

    signals:
        void selectedIndexChanged(int index);
        void dismissPopupRequested();
        void completionAccepted(int completionID);
        void isActiveChanged(bool value);
        void completionsAvailable();

    protected:
        virtual int getCompletionsCount() = 0;
        virtual QString doGetCompletion(int completionID) = 0;
        virtual QAbstractItemModel *doGetCompletionsModel() = 0;
        virtual void doInitializeCompletions() = 0;
        virtual void clearCompletions() = 0;
        virtual int doAcceptCompletion(int index, bool withMetaAction) = 0;

    private:
        int m_SelectedIndex;
        bool m_IsActive;
    };
}

#endif // AUTOCOMPLETEMODEL_H
