/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include <QObject>
#include <stack>
#include <memory>
#include <QMutex>
#include "../Commands/commandmanager.h"
#include "../Common/baseentity.h"
#include "iundoredomanager.h"

namespace UndoRedo {
    class HistoryItem;

    class UndoRedoManager:
            public QObject,
            public Common::BaseEntity,
            public IUndoRedoManager
    {
        Q_OBJECT
        Q_PROPERTY(bool canUndo READ getCanUndo NOTIFY canUndoChanged)
        Q_PROPERTY(QString undoDescription READ getUndoDescription NOTIFY undoDescriptionChanged)
    public:
        UndoRedoManager(QObject *parent=0):
            QObject(parent),
            Common::BaseEntity()
        {}

        virtual ~UndoRedoManager();

    public:
        bool getCanUndo() const { return !m_HistoryStack.empty(); }

    signals:
        void canUndoChanged();
        void undoDescriptionChanged();
        void itemRecorded();
        void undoStackEmpty();
        void actionUndone(int commandID);

    private:
        QString getUndoDescription() const { return m_HistoryStack.empty() ? "" : m_HistoryStack.top()->getDescription(); }

    public:
        virtual void recordHistoryItem(std::unique_ptr<UndoRedo::IHistoryItem> &historyItem) override;
        Q_INVOKABLE bool undoLastAction();
        Q_INVOKABLE void discardLastAction();

    private:
        // stack for future todos
        std::stack<std::unique_ptr<IHistoryItem> > m_HistoryStack;
        QMutex m_Mutex;
    };
}

#endif // UNDOREDOMANAGER_H
