/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworksviewmodel.h"
#include <QVector>
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"
#include "imageartwork.h"
#include "videoartwork.h"

namespace Models {
    ArtworksViewModel::ArtworksViewModel(QObject *parent):
        AbstractListModel(parent)
    {
    }

    void ArtworksViewModel::setArtworks(MetadataIO::WeakArtworksSnapshot &weakSnapshot) {
        LOG_INFO << weakSnapshot.size() << "artworks";
        if (weakSnapshot.empty()) { return; }

        MetadataIO::ArtworksSnapshot::Container rawSnapshot;
        rawSnapshot.reserve(weakSnapshot.size());

        for (auto *artwork: weakSnapshot) {
            rawSnapshot.emplace_back(new ArtworkElement(artwork));
        }

        beginResetModel();
        {
            m_ArtworksSnapshot.set(rawSnapshot);
        }
        endResetModel();

        emit artworksCountChanged();
    }

    int ArtworksViewModel::getSelectedArtworksCount() const {
        int selectedCount = 0;
        size_t size = m_ArtworksSnapshot.size();
        for (size_t i = 0; i < size; i++) {
            if (getIsSelected(i)) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    void ArtworksViewModel::setArtworkSelected(int index, bool value) {
        if (index < 0 || index >= (int)m_ArtworksSnapshot.size()) {
            return;
        }

        setIsSelected(index, value);

        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        emit selectedArtworksCountChanged();
    }

    void ArtworksViewModel::unselectAllItems() {
        const size_t size = m_ArtworksSnapshot.size();
        for (size_t i = 0; i < size; i++) {
            setIsSelected(i, false);
        }

        emit dataChanged(this->index(0), this->index(rowCount() - 1), QVector<int>() << IsSelectedRole);
    }

    ArtworkElement *ArtworksViewModel::accessItem(size_t index) const {
        Q_ASSERT(index < m_ArtworksSnapshot.size());
        auto &locker = m_ArtworksSnapshot.at(index);
        auto element = std::dynamic_pointer_cast<ArtworkElement>(locker);
        Q_ASSERT(element);
        return element.get();
    }

    bool ArtworksViewModel::getIsSelected(size_t i) const {
        auto &locker = m_ArtworksSnapshot.at(i);
        std::shared_ptr<ArtworkElement> element = std::dynamic_pointer_cast<ArtworkElement>(locker);
        Q_ASSERT(element);
        bool result = element->getIsSelected();
        return result;
    }

    void ArtworksViewModel::setIsSelected(size_t i, bool value) {
        auto &locker = m_ArtworksSnapshot.at(i);
        std::shared_ptr<ArtworkElement> element = std::dynamic_pointer_cast<ArtworkElement>(locker);
        Q_ASSERT(element);
        element->setIsSelected(value);
    }

    ArtworkMetadata *ArtworksViewModel::getArtworkMetadata(size_t i) const {
        Q_ASSERT((i >= 0) && (i < m_ArtworksSnapshot.size()));
        return m_ArtworksSnapshot.get(i);
    }

    /*virtual*/
    bool ArtworksViewModel::doRemoveSelectedArtworks() {
        LOG_DEBUG << "#";

        const size_t size = m_ArtworksSnapshot.size();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            if (getIsSelected(i)) {
                indicesToRemove.append((int)i);
            }
        }

        const bool anyItemToRemove = !indicesToRemove.empty();
        if (anyItemToRemove) {
            LOG_INFO << "Removing" << indicesToRemove.size() << "item(s)";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);

            if (m_ArtworksSnapshot.empty()) {
                emit requestCloseWindow();
            }

            emit artworksCountChanged();
        }

        return anyItemToRemove;
    }

    void ArtworksViewModel::doResetModel() {
        LOG_DEBUG << "#";

        beginResetModel();
        {
            m_ArtworksSnapshot.clear();
        }
        endResetModel();
    }

    void ArtworksViewModel::processArtworks(std::function<bool (const ArtworkElement *element)> pred,
                                            std::function<void (size_t, ArtworkMetadata *)> action) const {
        LOG_DEBUG << "#";

        auto &rawSnapshot = m_ArtworksSnapshot.getRawData();
        const size_t size = rawSnapshot.size();
        for (size_t i = 0; i < size; i++) {
            auto &item = rawSnapshot.at(i);
            const std::shared_ptr<ArtworkElement> element = std::dynamic_pointer_cast<ArtworkElement>(item);
            Q_ASSERT(element);

            if (pred(element.get())) {
                action(i, item->getArtworkMetadata());
            }
        }
    }

    void ArtworksViewModel::processArtworksEx(std::function<bool (const ArtworkElement *element)> pred,
                                              std::function<bool (size_t, ArtworkMetadata *)> action) const {
        LOG_DEBUG << "#";
        bool canContinue = false;

        auto &rawSnapshot = m_ArtworksSnapshot.getRawData();
        const size_t size = rawSnapshot.size();
        for (size_t i = 0; i < size; i++) {
            auto &locker = rawSnapshot.at(i);
            std::shared_ptr<ArtworkElement> element = std::dynamic_pointer_cast<ArtworkElement>(locker);
            Q_ASSERT(element);

            if (pred(element.get())) {
                canContinue = action(i, locker->getArtworkMetadata());

                if (!canContinue) { break; }
            }
        }
    }

    int ArtworksViewModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ArtworksSnapshot.size();
    }

    QVariant ArtworksViewModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_ArtworksSnapshot.size()) { return QVariant(); }

        auto &item = m_ArtworksSnapshot.at(row);
        auto *artwork = item->getArtworkMetadata();

        switch (role) {
        case FilepathRole: return artwork->getFilepath();
        case IsSelectedRole: return accessItem(row)->getIsSelected();
        case HasVectorAttachedRole: {
            auto *imageArtwork = dynamic_cast<ImageArtwork*>(artwork);
            return (imageArtwork != nullptr) && (imageArtwork->hasVectorAttached());
        }
        case ThumbnailPathRole: return artwork->getThumbnailPath();
        case IsVideoRole: {
            auto *videoArtwork = dynamic_cast<VideoArtwork*>(artwork);
            return videoArtwork != nullptr;
        }
        default: return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworksViewModel::roleNames() const {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();
        names[FilepathRole] = "filepath";
        names[IsSelectedRole] = "isselected";
        names[HasVectorAttachedRole] = "hasvectorattached";
        names[ThumbnailPathRole] = "thumbpath";
        names[IsVideoRole] = "isvideo";
        return names;
    }

    bool ArtworksViewModel::removeUnavailableItems() {
        LOG_DEBUG << "#";

        bool anyUnavailable = false;
        QVector<int> indicesToRemove;
        const size_t size = m_ArtworksSnapshot.size();

        for (size_t i = 0; i < size; i++) {
            auto *artwork = m_ArtworksSnapshot.get(i);

            if (artwork->isUnavailable()) {
                indicesToRemove.append((int)i);
                anyUnavailable = true;
            }
        }

        if (anyUnavailable) {
            LOG_INFO << "Found" << indicesToRemove.length() << "unavailable item(s)";
            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);

            removeItemsAtIndices(rangesToRemove);

            if (m_ArtworksSnapshot.empty()) {
                emit requestCloseWindow();
            }

            emit artworksCountChanged();
        }

        return anyUnavailable;
    }

    void ArtworksViewModel::removeInnerItem(int row) {
        m_ArtworksSnapshot.remove(row);
    }
}
