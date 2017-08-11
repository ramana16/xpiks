/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QObject>
#include <QQmlEngine>
#include "../Models/artworkproxybase.h"
#include "../Common/basicmetadatamodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"

#ifndef SPELLCHECKUSERDICT_H
#define SPELLCHECKUSERDICT_H

namespace SpellCheck {
    class UserDictEditModel: public QObject, public Models::ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)

    public:
        UserDictEditModel(QObject *parent=0);

        virtual ~UserDictEditModel() {}

    signals:
        void keywordsCountChanged();

    protected:
        virtual void signalDescriptionChanged() override { /* BUMP */ }
        virtual void signalTitleChanged() override { /* BUMP */ }
        virtual void signalKeywordsCountChanged() override { emit keywordsCountChanged(); }

    public:
        Q_INVOKABLE void initializeModel();
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE void saveUserDict();


    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() override { return &m_BasicModel; }
        virtual Common::IMetadataOperator *getMetadataOperator() override { return &m_BasicModel; }

    private:
        Common::Hold m_HoldPlaceholder;
        Common::BasicMetadataModel m_BasicModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
    };
}
#endif // SPELLCHECKUSERDICT_H
