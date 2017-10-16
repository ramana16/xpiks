/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "spellcheckitem.h"
#include "../Models/artworkmetadata.h"
#include <QStringList>
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"
#include "../Common/defines.h"
#include "../Common/basicmetadatamodel.h"
#include "../Helpers/stringhelper.h"

#define IMPOSSIBLE_TITLE_INDEX 100000
#define IMPOSSIBLE_DESCRIPTION_INDEX 200000

namespace SpellCheck {
    SpellCheckItemBase::~SpellCheckItemBase() {}

    void SpellCheckItemBase::accountResults() {
        for (auto &item: m_QueryItems) {
            m_SpellCheckResults.insert(item->m_Word,
                                       Common::WordAnalysisResult(item->m_Stem, item->m_IsCorrect, item->m_IsDuplicate));
        }
    }

    void SpellCheckItemBase::appendItem(const std::shared_ptr<SpellCheckQueryItem> &item) {
        m_QueryItems.push_back(item);
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags, Common::WordAnalysisFlags wordAnalysisFlag, int keywordIndex):
        SpellCheckItemBase(wordAnalysisFlag),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(true) {
        Q_ASSERT(Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Keywords));
        Q_ASSERT(spellCheckable != NULL);

        spellCheckable->acquire();

        QString keyword = m_SpellCheckable->retrieveKeyword(keywordIndex);
        if (!keyword.contains(QChar::Space)) {
            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, keyword));
            appendItem(queryItem);
        } else {
            QStringList parts = keyword.split(QChar::Space, QString::SkipEmptyParts);
            foreach(const QString &part, parts) {
                QString item = part.trimmed();
                std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, item));

                appendItem(queryItem);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags, Common::WordAnalysisFlags wordAnalysisFlags):
        SpellCheckItemBase(wordAnalysisFlags),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(false) {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> alwaysTrue = [](const QString &) {return true; };

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Keywords)) {
            QStringList keywords = spellCheckable->getKeywords();
            reserve(keywords.length());
            addWords(keywords, 0, alwaysTrue);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Description)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
            if (metadataModel != nullptr) {
                QStringList descriptionWords = metadataModel->getDescriptionWords();
                reserve(descriptionWords.length());
                addWords(descriptionWords, IMPOSSIBLE_DESCRIPTION_INDEX, alwaysTrue);
            }
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Title)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
            if (metadataModel != nullptr) {
                QStringList titleWords = metadataModel->getTitleWords();
                reserve(titleWords.length());
                addWords(titleWords, IMPOSSIBLE_TITLE_INDEX, alwaysTrue);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, const QStringList &keywordsToCheck, Common::WordAnalysisFlags wordAnalysisFlags):
        SpellCheckItemBase(wordAnalysisFlags),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(Common::SpellCheckFlags::All),
        m_OnlyOneKeyword(false)
    {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> containsFunc = [&keywordsToCheck](const QString &word) {
                                                                     bool contains = false;

                                                                     for (auto &item: keywordsToCheck) {
                                                                         if (word.contains(item, Qt::CaseInsensitive)) {
                                                                             contains = true;
                                                                             break;
                                                                         }
                                                                     }

                                                                     return contains;
                                                                 };

        QStringList keywords = spellCheckable->getKeywords();
        reserve(keywords.length());
        addWords(keywords, 0, containsFunc);

        std::function<bool (const QString &word)> sameKeywordFunc = [&keywordsToCheck](const QString &word) {
                                                                        bool match = false;

                                                                        for (auto &item: keywordsToCheck) {
                                                                            if (QString::compare(word, item, Qt::CaseInsensitive) == 0) {
                                                                                match = true;
                                                                                break;
                                                                            }
                                                                        }

                                                                        return match;
                                                                    };

        Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
        if (metadataModel != nullptr) {
            QStringList descriptionWords = metadataModel->getDescriptionWords();
            reserve(descriptionWords.length());
            addWords(descriptionWords, IMPOSSIBLE_DESCRIPTION_INDEX, sameKeywordFunc);

            QStringList titleWords = metadataModel->getTitleWords();
            reserve(titleWords.length());
            addWords(titleWords, IMPOSSIBLE_TITLE_INDEX, sameKeywordFunc);
        }
    }

    SpellCheckItem::~SpellCheckItem() {
        LOG_INTEGRATION_TESTS << "Actually deleting SpellCheckItem";

        Q_ASSERT(m_SpellCheckable != nullptr);
        if (m_SpellCheckable->release()) {
            LOG_WARNING << "Item could have been removed";
        }
    }

    void SpellCheckItem::addWords(const QStringList &words, int startingIndex, const std::function<bool (const QString &word)> &pred) {
        int index = startingIndex;

        foreach(const QString &word, words) {
            if (!word.contains(QChar::Space)) {
                if (pred(word)) {
                    std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, word));
                    appendItem(queryItem);
                }
            } else {
                QStringList parts = word.split(QChar::Space, QString::SkipEmptyParts);
                foreach(const QString &part, parts) {
                    QString item = part.trimmed();

                    if (item.length() >= 2) {
                        if (pred(item)) {
                            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, item));
                            appendItem(queryItem);
                        }
                    }
                }
            }

            index++;
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const std::vector<std::shared_ptr<SpellCheckQueryItem> > &items = getQueries();

        // can be empty in case of clear command
        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Keywords) && !items.empty()) {
            m_SpellCheckable->setKeywordsSpellCheckResults(items);
        }

        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Description) ||
            Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Title)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(m_SpellCheckable);
            if (metadataModel != nullptr) {
                metadataModel->setSpellCheckResults(getHash(), m_SpellCheckFlags);
            }
        }

        int index = m_OnlyOneKeyword ? (int)items.front()->m_Index : -1;
        emit resultsReady(m_SpellCheckFlags, index);
    }

    ModifyUserDictItem::ModifyUserDictItem(const QString &keyword):
        m_ClearFlag(false)
    {
        Helpers::splitText(keyword, m_KeywordsToAdd);
        m_KeywordsToAdd.removeDuplicates();
    }

    ModifyUserDictItem::ModifyUserDictItem(bool clearFlag):
        m_ClearFlag(clearFlag)
    { }

    ModifyUserDictItem::ModifyUserDictItem(const QStringList &keywords):
        m_ClearFlag(true)
    {
        for (auto &keyword : keywords) {
            Helpers::splitText(keyword, m_KeywordsToAdd);
        }

        m_KeywordsToAdd.removeDuplicates();
    }
}
