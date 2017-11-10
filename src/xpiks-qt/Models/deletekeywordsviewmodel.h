/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DELETEKEYWORDSVIEWMODEL_H
#define DELETEKEYWORDSVIEWMODEL_H

#include <QQmlEngine>
#include <QHash>
#include "../Common/hold.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "../Models/artworksviewmodel.h"
#include "../KeywordsPresets/ipresetsmanager.h"

namespace Models {
    class DeleteKeywordsViewModel: public Models::ArtworksViewModel
    {
        Q_OBJECT
        Q_PROPERTY(int commonKeywordsCount READ getCommonKeywordsCount NOTIFY commonKeywordsCountChanged)
        Q_PROPERTY(int keywordsToDeleteCount READ getKeywordsToDeleteCount NOTIFY keywordsToDeleteCountChanged)
        Q_PROPERTY(bool caseSensitive READ getCaseSensitive WRITE setCaseSensitive NOTIFY caseSensitiveChanged)

    public:
        DeleteKeywordsViewModel(QObject *parent=nullptr);
        virtual ~DeleteKeywordsViewModel() {}

    public:
        int getCommonKeywordsCount() { return m_CommonKeywordsModel.getKeywordsCount(); }
        int getKeywordsToDeleteCount() { return m_KeywordsToDeleteModel.getKeywordsCount(); }
        bool getCaseSensitive() const { return m_CaseSensitive; }
        void setCaseSensitive(bool value) {
            if (m_CaseSensitive != value) {
                m_CaseSensitive = value;
                emit caseSensitiveChanged();
            }
        }

    signals:
        void commonKeywordsCountChanged();
        void keywordsToDeleteCountChanged();
        void caseSensitiveChanged();

    public:
        virtual void setArtworks(MetadataIO::WeakArtworksSnapshot &artworks) override;
        virtual bool removeUnavailableItems() override;

#ifdef CORE_TESTS
    public:
        bool containsCommonKeyword(const QString &keyword) { return m_CommonKeywordsModel.containsKeyword(keyword); }
#endif

    protected:
        virtual bool doRemoveSelectedArtworks() override;
        virtual void doResetModel() override;

    public:
        Q_INVOKABLE QObject *getCommonKeywordsModel() {
            QObject *item = &m_CommonKeywordsModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE QObject *getKeywordsToDeleteModel() {
            QObject *item = &m_KeywordsToDeleteModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE void removeKeywordToDeleteAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeywordToDelete();
        Q_INVOKABLE void clearKeywordsToDelete();
        Q_INVOKABLE QString removeCommonKeywordAt(int keywordIndex);
        Q_INVOKABLE void appendKeywordToDelete(const QString &keyword);
        Q_INVOKABLE void pasteKeywordsToDelete(const QStringList &keywords);
        Q_INVOKABLE void deleteKeywords();
        Q_INVOKABLE bool addPreset(KeywordsPresets::ID_t presetID);

    private:
        void recombineKeywords();
        void fillKeywordsHash(QHash<QString, int> &keywordsHash);

    private:
        Common::Hold m_HoldForDeleters;
        Common::Hold m_HoldForCommon;
        Common::BasicKeywordsModel m_KeywordsToDeleteModel;
        Common::BasicKeywordsModel m_CommonKeywordsModel;
        bool m_CaseSensitive;
    };
}

#endif // DELETEKEYWORDSVIEWMODEL_H
