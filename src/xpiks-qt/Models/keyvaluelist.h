/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYVALUELIST_H
#define KEYVALUELIST_H

#include <QAbstractListModel>
#include <QHash>
#include <vector>
#include <memory>

namespace Models {
    class ArtworkMetadata;
    class ImageArtwork;
    class VideoArtwork;

    class KeyValueList: public QAbstractListModel
    {
        Q_OBJECT
    public:
        KeyValueList();

    public:
        enum KeyValueList_Roles {
            KeyRole = Qt::UserRole + 1,
            ValueRole
        };

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        virtual QString getKey(int index) const = 0;
        virtual QString getValue(int index) const = 0;
        virtual int getKeyValuesCount() const = 0;
    };

    class ArtworkPropertiesMap: public KeyValueList {
        Q_OBJECT
    public:
        ArtworkPropertiesMap(): m_IsImage(false) {}

    public:
        void updateProperties(ArtworkMetadata *metadata);

    private:
        void setForTheImage(ImageArtwork *imageArtwork);
        void setForTheVideo(VideoArtwork *videoArtwork);

    private:
        enum class ImageProperties {
            FilePathProperty,
            FileSizeProperty,
            ImageSizeProperty,
            DateTakenProperty,
            AttachedVectorProperty,
            NUM_IMAGE_PROPERTIES
        };

        enum class VideoProperties {
            FilePathProperty,
            FileSizeProperty,
            CodecProperty,
            FrameSizeProperty,
            VideoDurationProperty,
            FrameRateProperty,
            NUM_VIDEO_PROPERTIES
        };

        // KeyValueList interface
    protected:
        virtual QString getKey(int index) const override;
        virtual QString getValue(int index) const override;
        virtual int getKeyValuesCount() const override;

    private:
        QHash<int, QString> m_ValuesHash;
        bool m_IsImage;
    };
}

#endif // KEYVALUELIST_H
