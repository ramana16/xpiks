#include "presetkeywordsmodel.h"
#include "../Commands/commandmanager.h"

namespace  Presets {
    PresetKeywordsModel::PresetKeywordsModel(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity()
    {}

    bool PresetKeywordsModel::tryGetIndexFromName(const QString &presetName, int &result) {
        result = -1;
        int size = m_Presets.size();

        for (int i = 0; i < size; i++) {
            auto &item = m_Presets[i];
            if (item.m_PresetName == presetName) {
                result = i;
                return true;
            }
        }

        return false;
    }

    bool PresetKeywordsModel::tryGetNameFromIndex(int index, QString &name) {
        if (index < 0 || index >= m_Presets.size()) {
            return false;
        }

        name = m_Presets[index].m_PresetName;
        return true;
    }

    bool PresetKeywordsModel::tryGetPreset(int presetIndex, QStringList &keywords) {
        if (presetIndex < 0 || presetIndex >= m_Presets.size()) {
            return false;
        }

        keywords = m_Presets[presetIndex].m_KeywordsModel->getKeywords();
        return true;
    }

    void PresetKeywordsModel::removeItem(int row) {
        beginRemoveRows(QModelIndex(), row, row);
        removeInnerItem(row);
        endRemoveRows();
    }

    void PresetKeywordsModel::addItem() {
        int lastIndex = (int)getPresetsCount();
        Common::Hold placeHolder;
        std::shared_ptr<Common::BasicKeywordsModel> ptr(new Common::BasicKeywordsModel(placeHolder));

        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        m_Presets.push_back({std::move(ptr), QString("Undefined"), std::move(placeHolder)});
        endInsertRows();
        LOG_INFO << "#";
        emit dataChanged(this->index(lastIndex-1), this->index(lastIndex-1));
    }

    void PresetKeywordsModel::editKeyword(int index, int keywordIndex, const QString &replacement) {
        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_Presets[index].m_KeywordsModel;
            if (keywordsModel->editKeyword(keywordIndex, replacement)) {
                m_CommandManager->submitKeywordForSpellCheck(keywordsModel.get(), keywordIndex);
            }
        }
    }

    QString PresetKeywordsModel::removeKeywordAt(int index, int keywordIndex) {
        QString keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_Presets[index].m_KeywordsModel;
            if (keywordsModel->takeKeywordAt(keywordIndex, keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;
            }
        }

        return keyword;
    }

    void PresetKeywordsModel::appendKeyword(int index, const QString &keyword) {
        size_t size = getPresetsCount();

        LOG_DEBUG << "Model for item" << index;
        if (0 <= index && index < size) {
            auto &keywords = m_Presets[index].m_KeywordsModel;
            keywords->appendKeyword(keyword);
        }
    }

    QObject *PresetKeywordsModel::getKeywordsModel(int index) {
        size_t size = getPresetsCount();

        LOG_DEBUG << "Model for item" << index;
        if (0 <= index && index < size) {
            auto &keywords = m_Presets[index].m_KeywordsModel;
            QObject *item = keywords.get();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        } else {
            return NULL;
        }
    }

    void PresetKeywordsModel::saveToConfig() {
        auto *presetConfig = m_CommandManager->getpresetsModelConfig();

#ifndef CORE_TESTS
        presetConfig->saveFromModel(m_Presets);
#endif
    }

    void PresetKeywordsModel::loadFromConfigModel() {
        beginResetModel();
        auto *presetConfig = m_CommandManager->getpresetsModelConfig();
        auto &presetData = presetConfig->m_PresetData;
        int size = presetData.size();
        m_Presets.resize(size);
        for (int i = 0; i < size; i++) {
            auto item = presetData[i];
            auto &keywords = item.keys;
            auto &name = item.name;
            if (m_Presets[i].m_KeywordsModel) {
                m_Presets[i].m_KeywordsModel->clearKeywords();
                m_Presets[i].m_KeywordsModel->appendKeywords(keywords);
            } else {
                std::shared_ptr<Common::BasicKeywordsModel> ptr(new Common::BasicKeywordsModel(m_Presets[i].m_HoldPlaceholder));
                m_Presets[i].m_KeywordsModel.swap(ptr);
                m_Presets[i].m_KeywordsModel->appendKeywords(keywords);
            }

            m_Presets[i].m_PresetName = name;
        }

        endResetModel();
    }

    int PresetKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Presets.size();
    }

    QVariant PresetKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= m_Presets.size()) {
            return QVariant();
        }

        auto &name = m_Presets[row].m_PresetName;

        switch (role) {
            case NameRole:
                return name;
            default:
                return QVariant();
        }
    }

    bool PresetKeywordsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return false;
        }

        auto &name = m_Presets[row].m_PresetName;
        auto newName = value.toString();
        switch (role) {
            case NameRole:
                if (name == newName) {
                    return false;
                } else {
                    QString sanitized = newName.simplified();
                    if (sanitized == "") {
                        sanitized = "Undefined";
                    }

                    name = sanitized;
                    emit dataChanged(index, index);
                    return true;
                }

            default:
                return false;
        }
    }

    QHash<int, QByteArray> PresetKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        return roles;
    }

    void PresetKeywordsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < getPresetsCount());
        m_Presets.erase(m_Presets.begin() + row);
    }

    int FilteredPresetKeywordsModel::getOriginalIndex(int index)
    {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();

        return row;
    }

    QString FilteredPresetKeywordsModel::getName(int index)
    {
        int originalIndex = getOriginalIndex(index);
        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;
        presetsModel->tryGetNameFromIndex(originalIndex, name);
        return name;
    }

    const QString &FilteredPresetKeywordsModel::getSearchTerm() const { return m_SearchTerm; }

    void FilteredPresetKeywordsModel::setSearchTerm(const QString &value) {
        LOG_INFO << value;
        bool anyChangesNeeded = value != m_SearchTerm;

        if (anyChangesNeeded) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }
        invalidateFilter();
    }

    bool FilteredPresetKeywordsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        if (m_SearchTerm.trimmed().isEmpty()) {
            return true;
        }

        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;
        if (!presetsModel->tryGetNameFromIndex(sourceRow, name)) {
            return false;
        }

        return name.contains(m_SearchTerm, Qt::CaseInsensitive);
    }

    PresetKeywordsModel *FilteredPresetKeywordsModel::getPresetsModel() const
    {
        QAbstractItemModel *sourceItemModel = sourceModel();
        PresetKeywordsModel *presetsModel = dynamic_cast<PresetKeywordsModel *>(sourceItemModel);

        return presetsModel;
    }
}
