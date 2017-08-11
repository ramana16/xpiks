/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKSUGGESTIONMODEL_H
#define SPELLCHECKSUGGESTIONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <vector>
#include <memory>
#include <utility>
#include "../Common/baseentity.h"
#include "../Common/flags.h"
#include "../Common/imetadataoperator.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Common {
    class IMetadataOperator;
}

namespace SpellCheck {
    class SpellSuggestionsItem;
    class SpellCheckerService;

    typedef std::vector<std::shared_ptr<SpellSuggestionsItem> > SuggestionsVector;
    typedef std::vector<std::shared_ptr<KeywordSpellSuggestions> > KeywordsSuggestionsVector;

    class SpellCheckSuggestionModel : public QAbstractListModel, public Common::BaseEntity {
        Q_OBJECT
        Q_PROPERTY(int artworksCount READ getArtworksCount NOTIFY artworksCountChanged)
    public:
        SpellCheckSuggestionModel();
        virtual ~SpellCheckSuggestionModel();

    public:
        enum KeywordSpellSuggestions_Roles {
            WordRole = Qt::UserRole + 1,
            ReplacementIndexRole,
            ReplacementOriginRole
        };

    public:
        int getArtworksCount() const { return (int)m_ItemsPairs.size(); }

    public:
        Q_INVOKABLE QObject *getSuggestionItself(int index) const;
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void submitCorrections() const;
        Q_INVOKABLE void resetAllSuggestions();

    signals:
        void selectAllChanged();
        int artworksCountChanged();

    public:
        void setupModel(Common::IMetadataOperator *item, int index, Common::SuggestionFlags flags);
        void setupModel(std::vector<std::pair<Common::IMetadataOperator *, int> > &items, Common::SuggestionFlags flags);
#if defined(INTEGRATION_TESTS) || defined(CORE_TESTS)
        SpellSuggestionsItem *getItem(int i) const { return m_SuggestionsList.at(i).get(); }
#endif

    private:
        SuggestionsVector createSuggestionsRequests(Common::SuggestionFlags flags);
        bool processFailedReplacements(const SuggestionsVector &failedReplacements) const;
        SuggestionsVector setupSuggestions(const SuggestionsVector &items);

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        void updateItems() const;

    private:
        std::vector<std::shared_ptr<SpellSuggestionsItem> > m_SuggestionsList;
        std::vector<std::pair<Common::IMetadataOperator *, int> > m_ItemsPairs;
    };
}

#endif // SPELLCHECKSUGGESTIONMODEL_H
