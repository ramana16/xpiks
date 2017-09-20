/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYWORDSAUTOCOMPLETEMODEL_H
#define KEYWORDSAUTOCOMPLETEMODEL_H

#include "autocompletemodel.h"
#include <QAbstractListModel>
#include <QMutex>
#include <QSet>
#include <QAtomicInt>
#include <vector>
#include <memory>
#include "completionquery.h"

namespace AutoComplete {
    class KeywordsCompletionsModel: public QAbstractListModel {
        Q_OBJECT
    public:
        KeywordsCompletionsModel();

    public:
        enum AutoCompleteModelRoles {
            IsPresetRole = Qt::UserRole + 1,
        };

    public:
        void setKeywordCompletions(const std::vector<CompletionResult> &completions);
        void setPresetCompletions(const std::vector<CompletionResult> &completions);
        void setPresetsMembership(const std::vector<CompletionResult> &completions);

    public:
        std::shared_ptr<CompletionItem> getAcceptedCompletion(int completionID);
        QString getCompletion(int completionID);
        void sync();
        void clear();
        int acceptCompletion(int index, bool withMetaAction);

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override { Q_UNUSED(parent); return (int)m_CompletionList.size(); }
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

#ifdef INTEGRATION_TESTS
    public:
        bool containsWord(const QString &word) const;
        QStringList getLastGeneratedCompletions();
#endif
    signals:
        void updateRequired();

    private slots:
        void onUpdateRequired();

    private:
        QMutex m_CompletionsLock;
        QAtomicInt m_LastCompletionID;
        std::vector<std::shared_ptr<CompletionItem> > m_CompletionList;
        std::vector<std::shared_ptr<CompletionItem> > m_LastGeneratedCompletions;
        QHash<int, std::shared_ptr<CompletionItem> > m_AcceptedCompletions;
    };

    class KeywordsAutoCompleteModel: public AutoCompleteModel
    {
        Q_OBJECT
    public:
        KeywordsAutoCompleteModel();

    public:
        std::shared_ptr<CompletionItem> getAcceptedCompletion(int completionID) { return m_CompletionsModel.getAcceptedCompletion(completionID); }
        KeywordsCompletionsModel &getInnerModel() { return m_CompletionsModel; }

        // AutoCompleteModel interface
    protected:
        virtual int getCompletionsCount() override;
        virtual QString doGetCompletion(int completionID) override;
        virtual QAbstractItemModel *doGetCompletionsModel() override;
        virtual void doInitializeCompletions() override;
        virtual void clearCompletions() override;
        virtual int doAcceptCompletion(int index, bool withMetaAction) override;

    private:
        KeywordsCompletionsModel m_CompletionsModel;
    };
}

#endif // KEYWORDSAUTOCOMPLETEMODEL_H
