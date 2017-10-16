/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETKEYWORDSMODEL_H
#define PRESETKEYWORDSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QReadWriteLock>
#include <functional>
#include "../Common/basickeywordsmodel.h"
#include "../Common/baseentity.h"
#include "../Common/abstractlistmodel.h"
#include "ipresetsmanager.h"
#include "presetkeywordsmodelconfig.h"

namespace KeywordsPresets {
    struct PresetModel {
        PresetModel():
            m_KeywordsModel(m_Hold),
            m_PresetName(QObject::tr("Untitled"))
        {
        }

        PresetModel(const QString &name):
            m_KeywordsModel(m_Hold),
            m_PresetName(name)
        {}

        PresetModel(const QString &name, const QStringList &keywords):
            m_KeywordsModel(m_Hold),
            m_PresetName(name)
        {
            m_KeywordsModel.setKeywords(keywords);
        }

        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

        Common::BasicKeywordsModel m_KeywordsModel;
        QString m_PresetName;
        Common::Hold m_Hold;
    };

    class FilteredPresetKeywordsModel;

    class PresetKeywordsModel:
            public QAbstractListModel,
            public Common::BaseEntity,
            public IPresetsManager
    {
        Q_OBJECT

    public:
        PresetKeywordsModel(QObject *parent=0);
        virtual ~PresetKeywordsModel();

#ifdef INTEGRATION_TESTS
    public:
        PresetKeywordsModelConfig *getKeywordsModelConfig() { return &m_PresetsConfig; }
        void reload() { loadModelFromConfig(); }
#endif

    public:
        int getPresetsCount() const { return (int)m_PresetsList.size(); }

    public:
        void initializePresets();
        bool tryGetNameFromIndex(int index, QString &name);
        virtual bool tryGetPreset(int presetIndex, QStringList &keywords) override;
        void setName(int presetIndex, const QString &name);
        virtual bool tryFindSinglePresetByName(const QString &name, bool strictMatch, int &index) override;
        virtual void findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) override;
        virtual void findOrRegisterPreset(const QString &name, const QStringList &keywords, int &index) override;
        virtual void addOrUpdatePreset(const QString &name, const QStringList &keywords, int &index, bool &isAdded) override;
        virtual void requestBackup() override;
        virtual void refreshPresets() override;
        virtual void triggerPresetsUpdated() override;

        bool tryFindPresetByFullName(const QString &name, bool caseSensitive, int &index);
        void foreachPreset(const std::function<bool (int, PresetModel *)> &action);

        // safe and unsafe versions exist because of plugins which
        // can use presets manager in multithreaded way
    private:
        // TODO: fix this "friendship" when C++ will be an adequate language
        // and will allow access to protected methods from derived classes
        friend class FilteredPresetKeywordsModel;
        bool tryGetNameFromIndexUnsafe(int index, QString &name);
        bool tryGetPresetUnsafe(int presetIndex, QStringList &keywords);
        bool tryFindSinglePresetByNameUnsafe(const QString &name, bool strictMatch, int &index);
        void findPresetsByNameUnsafe(const QString &name, QVector<QPair<int, QString> > &results);
        void findOrRegisterPresetUnsafe(const QString &name, const QStringList &keywords, int &index);
        void addOrUpdatePresetUnsafe(const QString &name, const QStringList &keywords, int &index, bool &isAdded);
        bool tryFindPresetByFullNameUnsafe(const QString &name, bool caseSensitive, int &index);

    private:
        enum PresetKeywords_Roles {
            NameRole = Qt::UserRole + 1,
            EditNameRole,
            KeywordsCountRole,
            KeywordsStringRole
        };

    signals:
        void presetsUpdated();
        void backupRequest();

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;
        virtual void removeInnerItem(int row);

    public:
        Q_INVOKABLE void removeItem(int row);

        Q_INVOKABLE void addItem();
#if defined(CORE_TESTS) || defined(INTEGARTION_TESTS)
        void addItem(const QString &presetName, const QStringList &keywords);
#endif

        Q_INVOKABLE void editKeyword(int index, int keywordIndex, const QString &replacement);
        Q_INVOKABLE void removeKeywordAt(int index, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int index);
        Q_INVOKABLE void appendKeyword(int index, const QString &keyword);
        Q_INVOKABLE void pasteKeywords(int index, const QStringList &keywords);
        Q_INVOKABLE void plainTextEdit(int index, const QString &rawKeywords, bool spaceIsSeparator);
        Q_INVOKABLE QObject *getKeywordsModel(int index);
        Q_INVOKABLE void saveToConfig();

    private:
        /*Q_INVOKABLE*/ void loadModelFromConfig();

    public slots:
        void onBackupRequested();

    private slots:
        void onSavingTimerTriggered();

    private:
        void doLoadFromConfig();
        void removeAllPresets();

    private:
        PresetKeywordsModelConfig m_PresetsConfig;
        QReadWriteLock m_PresetsLock;
        std::vector<PresetModel *> m_PresetsList;
        std::vector<PresetModel *> m_Finalizers;
        QTimer m_SavingTimer;
    };

    class FilteredPresetKeywordsModel:
        public QSortFilterProxyModel
    {
    Q_OBJECT
    Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    public:
        Q_INVOKABLE int getOriginalIndex(int index);
        Q_INVOKABLE int getItemsCount() const { return rowCount(); }
        Q_INVOKABLE QString getName(int index);
    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);
    signals:
        void searchTermChanged(const QString &searchTerm);
        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    private:
        PresetKeywordsModel *getPresetsModel() const;
    private:
        QString m_SearchTerm;
    };
}
#endif // PRESETKEYWORDSMODEL_H
