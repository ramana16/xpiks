/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "combinedartworksmodel.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/combinededitcommand.h"
#include "../Commands/commandmanager.h"
#include "../Commands/commandbase.h"
#include "../Suggestion/keywordssuggestor.h"

namespace Models {
    void CombinedArtworksModel::initArtworks(const QList<ArtItemInfo *> &artworks)
    {
        int innerLength = m_ArtworksList.length();
        int start = innerLength == 0 ? 0 : innerLength - 1;
        int paramLength = artworks.length();
        if (paramLength > 0) {
            beginInsertRows(QModelIndex(), start, start + paramLength - 1);
            m_ArtworksList.append(artworks);
            endInsertRows();
        }
        m_IsModified = false;
    }

    void CombinedArtworksModel::recombineArtworks()
    {
        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        bool titleDiffer = false;
        bool authorDiffer = false;
        QString description;
        QString title;
        QString author;
        QSet<QString> commonKeywords;

        int artworksCount = m_ArtworksList.length();
        for (int i = 0; i < artworksCount; ++i) {
            ArtItemInfo *info = m_ArtworksList[i];
            ArtworkMetadata *metadata = info->getOrigin();

            if (!anyItemsProcessed) {
                description = metadata->getDescription();
                title = metadata->getTitle();
                author = metadata->getAuthor();
                commonKeywords.unite(metadata->getKeywordsSet());
                anyItemsProcessed = true;
                continue;
            }

            const QString &currDescription = metadata->getDescription();
            const QString &currTitle = metadata->getTitle();
            const QString &currAuthor = metadata->getAuthor();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            titleDiffer = titleDiffer || title != currTitle;
            authorDiffer = authorDiffer || author != currAuthor;
            commonKeywords.intersect(metadata->getKeywordsSet());
        }

        if (artworksCount > 0) {
            if (descriptionsDiffer) {
                description = "";
            }

            if (titleDiffer) {
                title = "";
            }

            if (authorDiffer) {
                author = "";
            }

            initDescription(description);
            initTitle(title);
            initAuthor(author);

            if (!m_IsModified) {
                initKeywords(commonKeywords.toList());
            }
        }
    }

    void CombinedArtworksModel::acceptSuggestedKeywords(const QStringList &keywords)  {
        foreach (const QString &keyword, keywords) {
            this->appendKeyword(keyword);
        }
    }

    void CombinedArtworksModel::resetModelData() {
        beginResetModel();
        qDeleteAll(m_ArtworksList);
        m_ArtworksList.clear();
        endResetModel();

        setDescription("");
        setAuthor("");
        setTitle("");
        setKeywords(QStringList());
        m_CommonKeywordsSet.clear();
    }

    void CombinedArtworksModel::createCombinedEditCommand(Commands::CombinedEditType commandType) const {
        Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                    commandType,
                    m_ArtworksList,
                    m_ArtworkDescription,
                    m_ArtworkTitle,
                    m_ArtworkAuthor,
                    m_CommonKeywordsModel.getKeywords());

        Commands::CommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
        Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
        m_CommandManager->updateArtworks(combinedResult->m_IndicesToUpdate);

        delete combinedResult;
    }

    QString CombinedArtworksModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        if (m_CommonKeywordsModel.removeKeyword(keywordIndex, keyword)) {
            m_CommonKeywordsSet.remove(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }

        return keyword;
    }

    void CombinedArtworksModel::removeLastKeyword() {
        QString keyword;
        if (m_CommonKeywordsModel.removeLastKeyword(keyword)) {
            m_CommonKeywordsSet.remove(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &word) {
        QString keyword = word.simplified();
        if (!keyword.isEmpty() && !m_CommonKeywordsSet.contains(keyword)) {
            m_CommonKeywordsModel.appendKeyword(keyword);
            m_CommonKeywordsSet.insert(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::pasteKeywords(const QStringList &keywords) {
        if (!keywords.empty()) {
            foreach (const QString &keyword, keywords) {
                if (!m_CommonKeywordsSet.contains(keyword)) {
                    m_CommonKeywordsSet.insert(keyword);
                    m_CommonKeywordsModel.appendKeyword(keyword);
                }
            }

            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::setArtworksSelected(int index, bool newState) {
        if (index < 0 || index >= m_ArtworksList.length()) {
            return;
        }

        m_ArtworksList[index]->setSelected(newState);
        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        emit selectedArtworksCountChanged();
    }

    void CombinedArtworksModel::removeSelectedArtworks() {
        int count = m_ArtworksList.length();
        QList<int> indicesToRemove;
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                indicesToRemove.append(i);
            }
        }

        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);

        recombineArtworks();
        emit artworksCountChanged();
    }

    void CombinedArtworksModel::saveSetKeywords() const {
        createCombinedEditCommand(Commands::SetEditType);
    }

    void CombinedArtworksModel::saveAddKeywords() const {
        createCombinedEditCommand(Commands::AppendEditType);
    }

    int CombinedArtworksModel::getSelectedArtworksCount() const {
        int selectedCount = 0;
        int count = m_ArtworksList.length();
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    int CombinedArtworksModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworksList.count();
    }

    QVariant CombinedArtworksModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworksList.count())
            return QVariant();

        ArtItemInfo *artItemInfo = m_ArtworksList.at(index.row());

        switch (role) {
        case PathRole:
            return artItemInfo->getOrigin()->getFilepath();
        case IsSelectedRole:
            return artItemInfo->isSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> CombinedArtworksModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void CombinedArtworksModel::removeInnerItem(int row) {
        ArtItemInfo *info = m_ArtworksList[row];
        delete info;
        m_ArtworksList.removeAt(row);
    }
}
