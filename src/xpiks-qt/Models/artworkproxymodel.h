/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKPROXYMODEL_H
#define ARTWORKPROXYMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>
#include <QString>
#include <QList>
#include <QSet>
#include <QSize>
#include <QQuickTextDocument>
#include <memory>
#include <vector>
#include "artworkmetadata.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/flags.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/hold.h"
#include "../Models/metadataelement.h"
#include "artworkproxybase.h"
#include "keyvaluelist.h"

namespace Models {
    class ArtworkProxyModel: public QObject, public ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(QString thumbPath READ getThumbPath NOTIFY thumbnailChanged)
        Q_PROPERTY(QString filePath READ getFilePath NOTIFY imagePathChanged)
        Q_PROPERTY(QString basename READ getBasename NOTIFY imagePathChanged)
        Q_PROPERTY(QString attachedVectorPath READ getAttachedVectorPath NOTIFY imagePathChanged)
        Q_PROPERTY(QSize imageSize READ retrieveImageSize NOTIFY imagePathChanged)
        Q_PROPERTY(QString fileSize READ retrieveFileSize NOTIFY imagePathChanged)
        Q_PROPERTY(QString dateTaken READ getDateTaken NOTIFY imagePathChanged)
        Q_PROPERTY(bool isVideo READ getIsVideo NOTIFY imagePathChanged)

    public:
        explicit ArtworkProxyModel(QObject *parent = 0);
        virtual ~ArtworkProxyModel();

    public:
        bool getIsVideo() const;
        QString getThumbPath() const { return m_ArtworkMetadata->getThumbnailPath(); }
        const QString &getFilePath() const { return m_ArtworkMetadata->getFilepath(); }
        QString getBasename() const { return m_ArtworkMetadata->getBaseFilename(); }

    public:
        virtual void setDescription(const QString &description) override;
        virtual void setTitle(const QString &title) override;
        virtual void setKeywords(const QStringList &keywords) override;

    signals:
        void imagePathChanged();
        void thumbnailChanged();
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();
        void completionsAvailable();
        void itemBecomeUnavailable();
        void warningsCouldHaveChanged(size_t originalIndex);
        void duplicatesCouldHaveChanged(size_t originalIndex);
        void spellingRehighlightRequired();

    protected:
        virtual void signalDescriptionChanged() override { emit descriptionChanged(); }
        virtual void signalTitleChanged() override { emit titleChanged(); }
        virtual void signalKeywordsCountChanged() override { emit keywordsCountChanged(); }

    public slots:
        void userDictUpdateHandler(const QStringList &keywords, bool overwritten);
        void userDictClearedHandler();
        void afterSpellingErrorsFixedHandler();
        void spellCheckErrorsChangedHandler();
        void itemUnavailableHandler(size_t index);

    public:
        Q_INVOKABLE void updateKeywords() { signalKeywordsCountChanged(); }
        Q_INVOKABLE void editKeyword(int index, const QString &replacement);
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE QString getKeywordsString();
        Q_INVOKABLE void suggestCorrections();
        Q_INVOKABLE void setupDuplicatesModel();
        Q_INVOKABLE void initDescriptionHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void spellCheckDescription();
        Q_INVOKABLE void spellCheckTitle();
        Q_INVOKABLE void plainTextEdit(const QString &rawKeywords, bool spaceIsSeparator=false);
        Q_INVOKABLE bool hasTitleWordSpellError(const QString &word);
        Q_INVOKABLE bool hasDescriptionWordSpellError(const QString &word);
        // --
        Q_INVOKABLE void setSourceArtwork(QObject *artworkMetadata);
        // --
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE QObject *getPropertiesMap() {
            QObject *item = &m_PropertiesMap;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE QSize retrieveImageSize() const;
        Q_INVOKABLE QString retrieveFileSize() const;
        Q_INVOKABLE QString getDateTaken() const;
        Q_INVOKABLE QString getAttachedVectorPath() const;
        Q_INVOKABLE void expandPreset(int keywordIndex, int presetIndex);
        Q_INVOKABLE void expandLastKeywordAsPreset();
        Q_INVOKABLE void addPreset(int presetIndex);
        Q_INVOKABLE void initSuggestion();
        Q_INVOKABLE void registerAsCurrentItem();
        Q_INVOKABLE void copyToQuickBuffer();
        Q_INVOKABLE void generateCompletions(const QString &prefix);
        Q_INVOKABLE bool acceptCompletionAsPreset(int completionID);

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() override {
            Q_ASSERT(m_ArtworkMetadata != nullptr);
            return m_ArtworkMetadata->getBasicModel();
        }

        virtual Common::IMetadataOperator *getMetadataOperator() override {
            Q_ASSERT(m_ArtworkMetadata != nullptr);
            return m_ArtworkMetadata;
        }

        virtual Common::ID_t getSpecialItemID() override;

    private:
        void updateCurrentArtwork();
        void disconnectCurrentArtwork();
        void releaseCurrentArtwork();

    private:
        ArtworkPropertiesMap m_PropertiesMap;
        Models::ArtworkMetadata *m_ArtworkMetadata;
    };
}

#endif // ARTWORKPROXYMODEL_H
