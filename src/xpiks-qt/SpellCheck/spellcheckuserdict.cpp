#include "spellcheckuserdict.h"
#include "../Helpers/keywordshelpers.h"
#include "../Commands/commandmanager.h"
#include "spellcheckerservice.h"

namespace SpellCheck {
    SpellCheckUserDict::SpellCheckUserDict(QObject *parent):
        Common::AbstractListModel(parent)
    {}

    void SpellCheckUserDict::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        LOG_INFO << "#";
        int rangesLength = Helpers::getRangesLength(ranges);
        AbstractListModel::doRemoveItemsAtIndices(ranges, rangesLength);
    }

    void SpellCheckUserDict::removeInnerItem(int row) {
        QString removedKeyword;

        this->doTakeKeyword(row, removedKeyword);
        LOG_INTEGRATION_TESTS << "keyword:" << removedKeyword;
        Q_UNUSED(removedKeyword);
    }

    int SpellCheckUserDict::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsList.length();
    }

    QVariant SpellCheckUserDict::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= m_KeywordsList.length()) {
            return QVariant();
        }

        switch (role) {
            case KeywordRole:
                return m_KeywordsList.at(index.row());
            default:
                return QVariant();
        }
    }

    QString SpellCheckUserDict::removeKeywordAt(int keywordIndex)
    {
        QString removed;
        removeKeywordAt(keywordIndex, removed);
        return removed;
    }

    void SpellCheckUserDict::removeLastKeyword()
    {
        QString removed;
        removeLastKeyword(removed);
        Q_UNUSED(removed);
    }

    void SpellCheckUserDict::pasteKeywords(const QStringList &keywords)
    {
        appendKeywords(keywords);
    }

    void SpellCheckUserDict::appendKeyword(const QString &keyword) {
        const QString &sanitizedKeyword = keyword.simplified();
        LOG_INFO << sanitizedKeyword;

        if (canBeAdded(sanitizedKeyword)) {
            int keywordsCount = m_KeywordsList.length();

            beginInsertRows(QModelIndex(), keywordsCount, keywordsCount);
            m_KeywordsList.append(sanitizedKeyword);
            m_KeywordsSet.insert(sanitizedKeyword.toLower());
            endInsertRows();
        }
    }

    bool SpellCheckUserDict::removeKeywordAt(int index, QString &removedKeyword) {
        bool result = false;
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                beginRemoveRows(QModelIndex(), index, index);
                doTakeKeyword(index, removedKeyword);
                endRemoveRows();
                result = true;
            }
        }
        return result;
    }

    bool SpellCheckUserDict::removeLastKeyword(QString &removedKeyword) {
        bool result = false;

        {
            if (m_KeywordsList.length() > 0) {
                int index = m_KeywordsList.length() - 1;
                beginRemoveRows(QModelIndex(), index, index);
                doTakeKeyword(index, removedKeyword);
                endRemoveRows();
                result = true;
            }
        }

        return result;
    }

    int SpellCheckUserDict::appendKeywords(const QStringList &keywordsList) {
        QStringList keywordsToAdd;
        int appendedCount = 0, size = keywordsList.length();

        keywordsToAdd.reserve(size);
        QSet<QString> accountedKeywords;

        for (int i = 0; i < size; ++i) {
            const QString &keyword = keywordsList.at(i);
            const QString &sanitizedKeyword = keyword.simplified();

            if (canBeAdded(sanitizedKeyword) && !accountedKeywords.contains(sanitizedKeyword)) {
                keywordsToAdd.append(sanitizedKeyword);
                accountedKeywords.insert(sanitizedKeyword);
                appendedCount++;
            }
        }

        size = keywordsToAdd.size();
        Q_ASSERT(size == appendedCount);

        if (size > 0) {
            int rowsCount = m_KeywordsList.length();
            beginInsertRows(QModelIndex(), rowsCount, rowsCount + size - 1);

            for (int i = 0; i < size; ++i) {
                const QString &keywordToAdd = keywordsToAdd.at(i);
                m_KeywordsSet.insert(keywordToAdd.toLower());
                m_KeywordsList.append(keywordToAdd);
            }

            endInsertRows();
            emit keywordsCountChanged();
        }

        return appendedCount;
    }

    int SpellCheckUserDict::getKeywordsCount() {
        return m_KeywordsList.length();
    }

    void SpellCheckUserDict::clearModel() {
        if (!areKeywordsEmpty()) {
            auto *service = m_CommandManager->getSpellCheckerService();
            service->clearUserDictionary();
            doClearKeywords();
            emit keywordsCountChanged();
        }
    }

    void SpellCheckUserDict::saveToExternalModel()
    {
        LOG_INFO << m_KeywordsList;

        if (getKeywordsCount() == 0) return;

        auto *service = m_CommandManager->getSpellCheckerService();
        service->updateUserDictionary(m_KeywordsList);
    }

    bool SpellCheckUserDict::areKeywordsEmpty() {
        return m_KeywordsList.isEmpty();
    }

    void SpellCheckUserDict::doTakeKeyword(int index, QString &removedKeyword) {
        const QString &keyword = m_KeywordsList.at(index);

        m_KeywordsSet.remove(keyword);

        removedKeyword = m_KeywordsList.takeAt(index);
    }

    bool SpellCheckUserDict::doEditKeyword(int index, const QString &replacement) {
        bool result = false;

        LOG_INFO << "index:" << index << "replacement:" << replacement;
        QString sanitized = Helpers::doSanitizeKeyword(replacement);

        QString existing = m_KeywordsList.at(index);
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            if (!m_KeywordsSet.contains(sanitized)) {
                m_KeywordsSet.insert(sanitized);
                m_KeywordsList[index] = sanitized;
                m_KeywordsSet.remove(existing);
                LOG_INFO << "common case edit:" << existing << "->" << sanitized;

                result = true;
            } else {
                LOG_WARNING << "Attempt to rename keyword to existing one. Use remove instead!";
            }
        }

        return result;
    }

    bool SpellCheckUserDict::doClearKeywords() {
        bool anyKeywords = !m_KeywordsList.isEmpty();

        if (anyKeywords) {
            beginResetModel();
            m_KeywordsList.clear();
            endResetModel();

            m_KeywordsSet.clear();
        } else {
            Q_ASSERT(m_KeywordsSet.isEmpty());
        }

        return anyKeywords;
    }

    bool SpellCheckUserDict::isEmpty() {
        return m_KeywordsList.isEmpty();
    }

    bool SpellCheckUserDict::canBeAdded(const QString &keyword) const {
        bool isValid = Helpers::isValidKeyword(keyword);
        bool result = isValid && !m_KeywordsSet.contains(keyword.toLower());

        return result;
    }

    bool SpellCheckUserDict::hasKeyword(const QString &keyword) {
        return !canBeAdded(keyword.simplified());
    }

    bool SpellCheckUserDict::canEditKeyword(int index, const QString &replacement) {
        bool result = false;

        LOG_INFO << "index:" << index << "replacement:" << replacement;

        QString sanitized = Helpers::doSanitizeKeyword(replacement);
        QString existing = m_KeywordsList.at(index);
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            if (!m_KeywordsSet.contains(sanitized)) {
                result = true;
            }
        }

        return result;
    }

    QHash<int, QByteArray> SpellCheckUserDict::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        return roles;
    }
}
