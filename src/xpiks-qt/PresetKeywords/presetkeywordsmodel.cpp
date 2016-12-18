#include "presetkeywordsmodel.h"
#include "../Commands/commandmanager.h"

namespace  Preset {

    PresetKeywordsModel::PresetKeywordsModel(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity()
    {}

    int PresetKeywordsModel::getIndexFromName(const QString &presetName)
    {
        int result = -1;
        int size = m_Presets.size();
        for (int i =0; i < size; i++){
            auto & item = m_Presets[i];
            if (item.name == presetName){
                result = i;
                break;
            }
        }
        return result;
    }

    QString PresetKeywordsModel::getNameFromIndex(int index)
    {
        if (index < 0 || index >= m_Presets.size()){
            return QString();
        }
        return m_Presets[index].name;
    }

    QStringList PresetKeywordsModel::getKeywords(int index)
    {
        if (index < 0 || index >= m_Presets.size()){
            return QStringList();
        }
        return m_Presets[index].keys->getKeywords();
    }


    void PresetKeywordsModel::removeItem(int row) {
        beginRemoveRows(QModelIndex(), row, row);
        removeInnerItem(row);
        endRemoveRows();
    }

    void PresetKeywordsModel::addItem() {
        int lastIndex = (int)getPresetsCount();
        std::shared_ptr<Common::BasicKeywordsModel> ptr(new Common::BasicKeywordsModel(m_HoldPlaceholder));
        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        m_Presets.push_back({{},QString("Undefined")});
        m_Presets.last().keys.swap(ptr);
        endInsertRows();
        LOG_INFO << "";
        emit dataChanged(this->index(m_Presets.size()-1),this->index(m_Presets.size()-1));
    }

    void PresetKeywordsModel::editKeyword(int index, int keywordIndex, const QString &replacement) {
        if (0 <= index && index < getPresetsCount()) {
            auto & keywordsModel = m_Presets[index].keys;
            if (keywordsModel->editKeyword(keywordIndex, replacement)) {
                m_CommandManager->submitKeywordForSpellCheck(keywordsModel.get(), keywordIndex);
            }
        }
    }

    QString PresetKeywordsModel::removeKeywordAt(int index, int keywordIndex)
    {
        QString keyword;
        if (0 <= index && index < getPresetsCount()) {
            auto & keywordsModel = m_Presets[index].keys;
            if (keywordsModel->takeKeywordAt(keywordIndex, keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;
            }
        }
     return keyword;
    }

    void PresetKeywordsModel::plainTextEdit(int index, const QString &rawKeywords, bool spaceSeparator) {
        LOG_DEBUG << "Plain text edit for item" << index;
        size_t size = getPresetsCount();
        if (0 <= index && index < size) {
            QRegExp regExp;
            if (spaceSeparator == true) {
                regExp = QRegExp("[,\\s]");
            } else {
                regExp = QRegExp(",");
            }

            QStringList keywords = rawKeywords.trimmed().split(regExp, QString::SkipEmptyParts);
            m_Presets[index].keys->setKeywords(keywords);
            emit dataChanged(this->index(index),this->index(index));
        }
    }

    void PresetKeywordsModel::appendKeyword(int index, const QString &keyword)
    {
        size_t size = getPresetsCount();
        LOG_DEBUG << "Model for item" << index;
        if (0 <= index && index < size) {
            auto &keywords = m_Presets[index].keys;
            keywords->appendKeyword(keyword);
        }
    }

    QObject *PresetKeywordsModel::getKeywordsModel(int index) {
        size_t size = getPresetsCount();
        LOG_DEBUG << "Model for item" << index;
        if (0 <= index && index < size) {
             auto &keywords = m_Presets[index].keys;
             QObject *item = keywords.get();
             QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
             return item;
         } else {
             return NULL;
        }
    }

    void PresetKeywordsModel::saveToConfig() {
        auto presetConfig = m_CommandManager->getPresetModelConfig();

       #ifndef CORE_TESTS
        presetConfig->saveFromModel(m_Presets);
       #endif
    }

    void PresetKeywordsModel::resetModel()
    {
        auto presetConfig = m_CommandManager->getPresetModelConfig();
        auto & presetData = presetConfig->m_PresetData;
        int size = presetData.size();
        LOG_WARNING<<"reset preset Model "<<size;
        m_Presets.resize(size);
        for (int i = 0; i < size; i++){
            auto item = presetData[i];
            auto & keywords = item.keys;
            auto &  name = item.name;
            if (m_Presets[i].keys){
                m_Presets[i].keys->clearKeywords();
                m_Presets[i].keys->appendKeywords(keywords);
            } else {
                 std::shared_ptr<Common::BasicKeywordsModel> ptr(new Common::BasicKeywordsModel(m_HoldPlaceholder));
                 m_Presets[i].keys.swap(ptr);
                 m_Presets[i].keys->appendKeywords(keywords);
            }
            m_Presets[i].name = name;
        }
        emit dataChanged(this->index(0),this->index(m_Presets.size()-1));
    }

    QStringList PresetKeywordsModel::getFilteredPresets(const QString &word) {
        QVector<int> indexes;
        QStringList presets;
        int size = m_Presets.size();
        for (int i = 0; i < size; i++) {
            auto &name = m_Presets[i].name;
            if (name.contains(word, Qt::CaseInsensitive)) {
                indexes.push_back(i);
                presets.push_back(name);
            }
        }

        return presets;
    }

    int PresetKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        LOG_DEBUG << "tasha ok" << m_Presets.size();
        return m_Presets.size();
    }

    QVariant PresetKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= m_Presets.size()) {
            return QVariant();
        }

        auto &name = m_Presets[row].name;

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

        auto &name = m_Presets[row].name;
        auto newName = value.toString();
        switch (role) {
            case NameRole:
                if (name == newName) {
                    return false;
                } else {
                    QString sanitized = newName.simplified();
                    if (sanitized == ""){
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

    void PresetKeywordsModel::removeInnerItem(int row)
    {
        Q_ASSERT(row >= 0 && row < getPresetsCount());
         m_Presets.erase(m_Presets.begin() + row);

    }
}
