/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "duplicatesreviewmodel.h"
#include <QQuickTextDocument>
#include <QFileInfo>
#include "../SpellCheck/duplicateshighlighter.h"
#include "../Common/basicmetadatamodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/stringhelper.h"
#include "../Common/keyword.h"
#include "../Models/videoartwork.h"
#include "../Models/imageartwork.h"

namespace SpellCheck {
    DuplicatesReviewModel::DuplicatesReviewModel(QMLExtensions::ColorsModel *colorsModel):
        m_ColorsModel(colorsModel)
    {
        Q_ASSERT(colorsModel != nullptr);
    }

    void DuplicatesReviewModel::setupModel(Common::BasicMetadataModel *basicModel) {
        Q_ASSERT(m_DuplicatesList.empty());
        beginResetModel();
        {
            m_DuplicatesList.emplace_back(basicModel);
        }
        endResetModel();
    }

    void DuplicatesReviewModel::setupModel(const std::vector<Models::ArtworkMetadata *> &items) {
        Q_ASSERT(m_DuplicatesList.empty());
        beginResetModel();
        {
            m_DuplicatesList.reserve(items.size());
            for (auto &item: items) {
                m_DuplicatesList.emplace_back(item);
            }
        }
        endResetModel();
    }

    void DuplicatesReviewModel::clearDuplicates() {
        beginResetModel();
        {
            m_DuplicatesList.clear();
        }
        endResetModel();
    }

    void DuplicatesReviewModel::initTitleHighlighting(int index, QQuickTextDocument *document) {
#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return; }

        auto &item = m_DuplicatesList.at(index);
        auto *basicModel = item.m_BasicModel;
        Q_ASSERT(basicModel != nullptr);
        SpellCheck::SpellCheckItemInfo *spellCheckInfo = basicModel->getSpellCheckInfo();

        SpellCheck::DuplicatesHighlighter *highlighter = new SpellCheck::DuplicatesHighlighter(
                    document->textDocument(),
                    m_ColorsModel,
                    spellCheckInfo->getTitleErrors());

        QObject::connect(basicModel, &Common::BasicMetadataModel::hasDuplicatesChanged,
                         highlighter, &DuplicatesHighlighter::rehighlight);

        //QObject::connect(this, &DuplicatesReviewModel::rehighlightRequired,
        //                 highlighter, &DuplicatesHighlighter::rehighlight);

        Q_UNUSED(highlighter);
#else
        Q_UNUSED(index);
        Q_UNUSED(document);
#endif
    }

    void DuplicatesReviewModel::initDescriptionHighlighting(int index, QQuickTextDocument *document) {
#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return; }

        auto &item = m_DuplicatesList.at(index);
        auto *basicModel = item.m_BasicModel;
        Q_ASSERT(basicModel != nullptr);
        SpellCheck::SpellCheckItemInfo *spellCheckInfo = basicModel->getSpellCheckInfo();

        SpellCheck::DuplicatesHighlighter *highlighter = new SpellCheck::DuplicatesHighlighter(
                    document->textDocument(),
                    m_ColorsModel,
                    spellCheckInfo->getDescriptionErrors());

        QObject::connect(basicModel, &Common::BasicMetadataModel::hasDuplicatesChanged,
                         highlighter, &DuplicatesHighlighter::rehighlight);

        //QObject::connect(this, &DuplicatesReviewModel::rehighlightRequired,
        //                 highlighter, &DuplicatesHighlighter::rehighlight);

        Q_UNUSED(highlighter);
#else
        Q_UNUSED(index);
        Q_UNUSED(document);
#endif
    }

    void DuplicatesReviewModel::initKeywordsHighlighting(int index, QQuickTextDocument *document) {
#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return; }

        auto &item = m_DuplicatesList.at(index);
        auto *basicModel = item.m_BasicModel;
        Q_ASSERT(basicModel != nullptr);

        bool hasKeywordsDuplicates = basicModel->hasKeywordsDuplicates();

        SpellCheck::DuplicatesHighlighter *highlighter = new SpellCheck::DuplicatesHighlighter(
                    document->textDocument(),
                    m_ColorsModel,
                    nullptr, // highlight all words
                    hasKeywordsDuplicates);

        QObject::connect(basicModel, &Common::BasicMetadataModel::hasDuplicatesChanged,
                         highlighter, &DuplicatesHighlighter::keywordsDuplicatesChanged);

        Q_UNUSED(highlighter);
#else
        Q_UNUSED(index);
        Q_UNUSED(document);
#endif
    }

    QString DuplicatesReviewModel::getTitleDuplicates(int index) {
        LOG_FOR_DEBUG << index;
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return QString(); }

        auto &item = m_DuplicatesList.at(index);
        auto *basicModel = item.m_BasicModel;
        SpellCheck::SpellCheckItemInfo *spellCheckInfo = basicModel->getSpellCheckInfo();

        QString title = basicModel->getTitle();
        QString result;

        if (spellCheckInfo->anyTitleDuplicates()) {
            std::vector<int> hits;
            QString titleLower = title.toLower();

            Helpers::foreachWord(titleLower,
                                 [&spellCheckInfo](const QString &word) {
                return spellCheckInfo->hasTitleDuplicate(word);
            },
            [&hits](int start, int length, const QString &) {
                hits.push_back(start + length/2);
            });

            result = Helpers::getUnitedHitsString(title, hits, DUPLICATEOFFSET);
        } else {
            if (title.size() > PREVIEWOFFSET*2) {
                result = title.left(PREVIEWOFFSET*2) + " ...";
            } else {
                result = title;
            }
        }

        return result;
    }

    QString DuplicatesReviewModel::getDescriptionDuplicates(int index) {
        LOG_FOR_DEBUG << index;
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return QString(); }

        auto &item = m_DuplicatesList.at(index);
        auto *basicModel = item.m_BasicModel;
        SpellCheck::SpellCheckItemInfo *spellCheckInfo = basicModel->getSpellCheckInfo();

        QString description = basicModel->getDescription();
        QString result;

        if (spellCheckInfo->anyDescriptionDuplicates()) {
            std::vector<int> hits;
            QString descriptionLower = description.toLower();

            Helpers::foreachWord(descriptionLower,
                                 [&spellCheckInfo](const QString &word) {
                return spellCheckInfo->hasDescriptionDuplicate(word);
            },
            [&hits](int start, int length, const QString &) {
                hits.push_back(start + length/2);
            });

            result = Helpers::getUnitedHitsString(description, hits, DUPLICATEOFFSET);
        } else {
            if (description.size() > PREVIEWOFFSET*2) {
                result = description.left(PREVIEWOFFSET*2) + " ...";
            } else {
                result = description;
            }
        }

        return result;
    }

    QString DuplicatesReviewModel::getKeywordsDuplicates(int index) {
        LOG_FOR_DEBUG << index;
        if ((index < 0) || (index >= (int)m_DuplicatesList.size())) { return QString(); }

        auto &item = m_DuplicatesList.at(index);
        Common::BasicMetadataModel *basicModel = item.m_BasicModel;
        std::vector<Common::KeywordItem> duplicatedKeywords = basicModel->retrieveDuplicatedKeywords();

        QString text;

        if (!duplicatedKeywords.empty()) {
            QStringList keywords;
            keywords.reserve((int)duplicatedKeywords.size());
            for (auto &item: duplicatedKeywords) {
                keywords.append(item.m_Word);
            }

            text = keywords.join(", ");
        } else {
            QStringList keywords = basicModel->getKeywords();
            if (keywords.length() > DUPLICATESKEYWORDSCOUNT) {
                QStringList part = keywords.mid(0, DUPLICATESKEYWORDSCOUNT);
                text = part.join(", ") + " ...";
            } else {
                text = keywords.join(", ");
            }
        }

        return text;
    }

    void DuplicatesReviewModel::clearModel() {
        clearDuplicates();
        m_PendingUpdates.clear();
    }

    void DuplicatesReviewModel::processPendingUpdates() {
        LOG_DEBUG << "#";
        if (m_DuplicatesList.empty()) {
            m_PendingUpdates.clear();
            return;
        }

        QSet<size_t> indicesSet = m_PendingUpdates.toList().toSet();

        QVector<int> indicesToUpdate;
        const size_t size = m_DuplicatesList.size();
        indicesToUpdate.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            auto &item = m_DuplicatesList[i];
            if (item.m_ArtworkMetadata == nullptr) { continue; }

            const size_t index = item.m_ArtworkMetadata->getLastKnownIndex();
            if (indicesSet.contains(index)) {
                indicesToUpdate.append((int)i);
            }
        }

        LOG_INTEGR_TESTS_OR_DEBUG << indicesToUpdate;

        QVector<QPair<int, int> > ranges;
        qSort(indicesToUpdate);
        Helpers::indicesToRanges(indicesToUpdate, ranges);

        QVector<int> roles;
        roles << TriggerRole;

        for (auto &r: ranges) {
            QModelIndex indexFrom = this->index(r.first, 0);
            QModelIndex indexTo = this->index(r.second, 0);
            emit dataChanged(indexFrom, indexTo, roles);
        }

        m_PendingUpdates.clear();
    }

    int DuplicatesReviewModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_DuplicatesList.size();
    }

    QVariant DuplicatesReviewModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= (int)m_DuplicatesList.size())) { return QVariant(); }

        auto &item = m_DuplicatesList.at(row);

        switch (role) {
        case HasPathRole: {
            return item.m_ArtworkMetadata != nullptr;
        }
        case PathRole: {
            return item.m_ArtworkMetadata != nullptr ? item.m_ArtworkMetadata->getThumbnailPath() : "";
        }
        case OriginalIndexRole: {
            return (int)(item.m_ArtworkMetadata != nullptr ? item.m_ArtworkMetadata->getLastKnownIndex() : 0);
        }
        case HasVectorAttachedRole: {
            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(item.m_ArtworkMetadata);
            return (image != NULL) && image->hasVectorAttached();
        }
        case BaseFilenameRole:
            return item.m_ArtworkMetadata != nullptr ? item.m_ArtworkMetadata->getBaseFilename() : "";
        case IsVideoRole: {
            bool isVideo = dynamic_cast<Models::VideoArtwork*>(item.m_ArtworkMetadata) != nullptr;
            return isVideo;
        }
        case TriggerRole: {
            return QString();
        }
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> DuplicatesReviewModel::roleNames() const {
        QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
        roleNames[HasPathRole] = "hasthumbnail";
        roleNames[PathRole] = "thumbpath";
        roleNames[OriginalIndexRole] = "originalIndex";
        roleNames[IsVideoRole] = "isvideo";
        roleNames[HasVectorAttachedRole] = "hasvectorattached";
        roleNames[BaseFilenameRole] = "basefilename";
        roleNames[TriggerRole] = "dtrigger";
        return roleNames;
    }

    void DuplicatesReviewModel::onDuplicatesCouldHaveChanged(size_t originalIndex) {
        LOG_DEBUG << originalIndex;
        m_PendingUpdates.push_back(originalIndex);
    }
}
