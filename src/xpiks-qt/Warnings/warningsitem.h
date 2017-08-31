/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WARNINGSQUERYITEM
#define WARNINGSQUERYITEM

#include <QStringList>
#include <QString>
#include <QSet>
#include "../Helpers/stringhelper.h"
#include "../Common/flags.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "iwarningsitem.h"

namespace Warnings {
    class EmptyWarningsItem: public IWarningsItem {
    public:
        virtual void submitWarnings(Common::flag_t) override { /* BUMP */ }
    };

    class WarningsItem: public IWarningsItem {
    public:
        WarningsItem(Models::ArtworkMetadata *checkableItem, Common::WarningsCheckFlags checkingFlags = Common::WarningsCheckFlags::All):
            m_CheckableItem(checkableItem),
            m_CheckingFlags(checkingFlags)
        {
            checkableItem->acquire();
            m_Description = checkableItem->getDescription();
            m_Title = checkableItem->getTitle();
            m_KeywordsSet = checkableItem->getKeywordsSet();
        }

        virtual ~WarningsItem() {
            if (m_CheckableItem->release()) {
                LOG_WARNING << "Item #" << m_CheckableItem->getItemID() << "could have been removed";
            }
        }

    public:
        virtual void submitWarnings(Common::flag_t warningsFlags) override {
            if (m_CheckingFlags == Common::WarningsCheckFlags::All) {
                m_CheckableItem->setWarningsFlags(warningsFlags);
            } else {
                Common::WarningFlags flagsToDrop = Common::WarningFlags::None;

                switch (m_CheckingFlags) {
                case Common::WarningsCheckFlags::Description:
                    flagsToDrop = Common::WarningFlags::DescriptionGroup;
                    break;
                case Common::WarningsCheckFlags::Keywords:
                    flagsToDrop = Common::WarningFlags::KeywordsGroup;
                    break;
                case Common::WarningsCheckFlags::Title:
                    flagsToDrop = Common::WarningFlags::TitleGroup;
                    break;
                case Common::WarningsCheckFlags::Spelling:
                    flagsToDrop = Common::WarningFlags::SpellingGroup;
                    break;
                case Common::WarningsCheckFlags::All:
                    // to make compiler happy
                    break;
                default:
                    break;
                }

                m_CheckableItem->dropWarningsFlags((Common::flag_t)flagsToDrop);
                m_CheckableItem->addWarningsFlags((Common::flag_t)warningsFlags);
            }
        }

        bool needCheckAll() const { return m_CheckingFlags == Common::WarningsCheckFlags::All; }
        Common::WarningsCheckFlags getCheckingFlags() const { return m_CheckingFlags; }
        const QString &getDescription() const { return m_Description; }
        const QString &getTitle() const { return m_Title; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }

        QStringList getDescriptionWords() const {
            QStringList words;
            Helpers::splitText(m_Description, words);
            return words;
        }

        QStringList getTitleWords() const {
            QStringList words;
            Helpers::splitText(m_Title, words);
            return words;
        }

        Models::ArtworkMetadata *getCheckableItem() const { return m_CheckableItem; }

    private:
        Models::ArtworkMetadata *m_CheckableItem;
        QString m_Description;
        QString m_Title;
        QSet<QString> m_KeywordsSet;
        Common::WarningsCheckFlags m_CheckingFlags;
    };
}

#endif // WARNINGSQUERYITEM

