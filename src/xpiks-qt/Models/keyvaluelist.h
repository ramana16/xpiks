/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
            FPSProperty,
            AspectRatioProperty,
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
