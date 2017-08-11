/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#ifndef SUGGESTIONARTWORK_H
#define SUGGESTIONARTWORK_H

#include <QVector>
#include <QString>
#include <QSet>

namespace Suggestion {
    class SuggestionArtwork
    {
    public:
        SuggestionArtwork(const QString &url, const QString &title, const QString &description, const QStringList &keywords, bool isLocal = true) :
            m_KeywordsSet(keywords.toSet()),
            m_Title(title),
            m_Description(description),
            m_ExternalUrl(""),
            m_IsSelected(false)
        {
            if (isLocal) {
                m_Url = QLatin1String("image://cached/") + url;
            } else {
                m_Url = url;
            }
        }

        SuggestionArtwork(const QString &url, const QString &externalUrl, const QString &title, const QString &description, const QStringList &keywords) :
            m_KeywordsSet(keywords.toSet()),
            m_Title(title),
            m_Description(description),
            m_Url(url),
            m_ExternalUrl(externalUrl),
            m_IsSelected(false)
        {
        }

    public:
        const QString &getUrl() const { return m_Url; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        const QString &getTitle() const { return m_Title; }
        const QString &getDescription() const { return m_Description; }
        bool getIsSelected() const { return m_IsSelected; }
        const QString &getExternalUrl() const { return m_ExternalUrl; }

    public:
        void setIsSelected(bool isSelected) { m_IsSelected = isSelected; }

    private:
        QSet<QString> m_KeywordsSet;
        QString m_Title;
        QString m_Description;
        QString m_Url;
        QString m_ExternalUrl;
        bool m_IsSelected;
    };
}

#endif // SUGGESTIONARTWORK_H
