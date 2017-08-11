/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FINDANDREPLACEMODEL_H
#define FINDANDREPLACEMODEL_H

#include "../Common/baseentity.h"
#include <QObject>
#include <QQuickTextDocument>
#include "../Models/previewmetadataelement.h"
#include "../Common/flags.h"
#include "../Common/iflagsprovider.h"

namespace Models {
    class FindAndReplaceModel:
        public QAbstractListModel,
        public Common::BaseEntity,
        public Common::IFlagsProvider
    {
        Q_OBJECT
        Q_PROPERTY(QString replaceFrom READ getReplaceFrom WRITE setReplaceFrom NOTIFY replaceFromChanged)
        Q_PROPERTY(QString replaceTo READ getReplaceTo WRITE setReplaceTo NOTIFY replaceToChanged)
        Q_PROPERTY(bool searchInTitle READ getSearchInTitle WRITE setSearchInTitle NOTIFY searchInTitleChanged)
        Q_PROPERTY(bool searchInDescription READ getSearchInDescription WRITE setSearchInDescription NOTIFY searchInDescriptionChanged)
        Q_PROPERTY(bool searchInKeywords READ getSearchInKeywords WRITE setSearchInKeywords NOTIFY searchInKeywordsChanged)
        Q_PROPERTY(bool caseSensitive READ getCaseSensitive WRITE setCaseSensitive NOTIFY caseSensitiveChanged)
        Q_PROPERTY(bool searchWholeWords READ getSearchWholeWords WRITE setSearchWholeWords NOTIFY searchWholeWordsChanged)
        Q_PROPERTY(int count READ getArtworksCount NOTIFY countChanged)

    public:
        FindAndReplaceModel(QMLExtensions::ColorsModel *colorsModel, QObject *parent=0);

        virtual ~FindAndReplaceModel() {}

    public:
        virtual Common::flag_t getFlags() const override { return (Common::flag_t)m_Flags; }
        const QString &getReplaceFrom() const{ return m_ReplaceFrom; }
        const QString &getReplaceTo() const { return m_ReplaceTo; }
        int getArtworksCount() const { return (int)m_ArtworksList.size(); }

        void setReplaceFrom(const QString &value) {
            if (value != m_ReplaceFrom) {
                m_ReplaceFrom = value;
                emit replaceFromChanged(value);
            }
        }

        void setReplaceTo(const QString &value) {
            if (value != m_ReplaceTo) {
                m_ReplaceTo = value;
                emit replaceToChanged(value);
            }
        }

    public:
        bool getSearchInTitle() const {
            return Common::HasFlag(m_Flags, Common::SearchFlags::Title);
        }

        void setSearchInTitle(bool value) {
            if (value != getSearchInTitle()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlags::Title);
                emit searchInTitleChanged(value);
            }
        }

        bool getSearchInDescription() const {
            return Common::HasFlag(m_Flags, Common::SearchFlags::Description);
        }

        void setSearchInDescription(bool value) {
            if (value != getSearchInDescription()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlags::Description);
                emit searchInDescriptionChanged(value);
            }
        }

        bool getSearchInKeywords() const {
            return Common::HasFlag(m_Flags, Common::SearchFlags::Keywords);
        }

        void setSearchInKeywords(bool value) {
            if (value != getSearchInKeywords()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlags::Keywords);
                emit searchInKeywordsChanged(value);
            }
        }

        bool getCaseSensitive() const {
            return Common::HasFlag(m_Flags, Common::SearchFlags::CaseSensitive);
        }

        void setCaseSensitive(bool value) {
            if (value != getCaseSensitive()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlags::CaseSensitive);
                emit caseSensitiveChanged(value);
            }
        }

        bool getSearchWholeWords() const {
            return Common::HasFlag(m_Flags, Common::SearchFlags::WholeWords);
        }

        void setSearchWholeWords(bool value) {
            if (value != getSearchWholeWords()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlags::WholeWords);
                emit searchWholeWordsChanged(value);
            }
        }

    public:
        enum FindAndReplaceModel_Roles {
            PathRole = Qt::UserRole + 1,
            IsSelectedRole,
            EditIsSelectedRole,
            HasTitleMatchRole,
            HasDescriptionMatchRole,
            HasKeywordsMatchRole
        };

    public:
        Q_INVOKABLE void initArtworksList();

#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
        Q_INVOKABLE void initHighlighting(QQuickTextDocument *document);
#endif
        Q_INVOKABLE QString getSearchTitle(int index);
        Q_INVOKABLE QString getSearchDescription(int index);
        Q_INVOKABLE QString getSearchKeywords(int index);
        Q_INVOKABLE void replace();
        Q_INVOKABLE void selectAll() { setAllSelected(true); }
        Q_INVOKABLE void unselectAll() { setAllSelected(false); }
        Q_INVOKABLE bool anySearchDestination() const;
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE void clearArtworks();

#ifdef INTEGRATION_TESTS
        void setItemSelected(int index, bool selected) {
            m_ArtworksList[index].setSelected(selected);
        }
#endif

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    signals:
        void replaceFromChanged(const QString &value);
        void replaceToChanged(const QString &value);
        void searchInTitleChanged(bool value);
        void searchInDescriptionChanged(bool value);
        void searchInKeywordsChanged(bool value);
        void caseSensitiveChanged(bool value);
        void searchWholeWordsChanged(bool value);
        void countChanged(int value);
        void allSelectedChanged();
        void replaceSucceeded();

    private:
        QString filterText(const QString &text);
        void setAllSelected(bool isSelected);
        void initDefaultFlags();
        void normalizeSearchCriteria();

    private:
        std::vector<Models::PreviewMetadataElement> m_ArtworksList;
        QString m_ReplaceFrom;
        QString m_ReplaceTo;
        QMLExtensions::ColorsModel *m_ColorsModel;
        Common::SearchFlags m_Flags;
    };
}
#endif // FINDANDREPLACEMODEL_H
