#ifndef PRESETKEYWORDSMODEL_H
#define PRESETKEYWORDSMODEL_H

#include "../Common/basickeywordsmodel.h"
#include "../Common/baseentity.h"
#include "../Common/abstractlistmodel.h"
#include <QAbstractListModel>
#include <QVector>

namespace Presets {

struct Preset {
   std::shared_ptr<Common::BasicKeywordsModel> m_KeywordsModel;
   QString m_PresetName;
};

class PresetKeywordsModel:
        public QAbstractListModel,
        public Common::BaseEntity
{
Q_OBJECT
    public:
        PresetKeywordsModel(QObject *parent = 0);
        size_t getPresetsCount() const {return m_Presets.size();}
        int getIndexFromName(const QString & presetName);
        QString getNameFromIndex(int index);
        bool tryGetPreset(int presetIndex, QStringList &keywords);

    public:
       enum PresetKeywords_Roles {
         NameRole = Qt::UserRole + 1
       };

        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    protected:
        virtual QHash<int, QByteArray> roleNames() const;
        virtual void removeInnerItem(int row);
   public:
        Q_INVOKABLE void removeItem(int row);
        Q_INVOKABLE void addItem();
        Q_INVOKABLE void editKeyword(int index, int keywordIndex, const QString &replacement);
        Q_INVOKABLE QString removeKeywordAt(int index, int keywordIndex);
        Q_INVOKABLE void appendKeyword(int index, const QString &keyword);
        Q_INVOKABLE QObject *getKeywordsModel(int index);
        Q_INVOKABLE void saveToConfig();
        Q_INVOKABLE void loadFromConfigModel();
        Q_INVOKABLE QStringList getFilteredPresets(const QString &  word);
    public slots:
       void onPresetsUpdated() { LOG_INFO << "loading Model"; loadFromConfigModel();}
    private:
        Common::Hold m_HoldPlaceholder;
        QVector<Preset> m_Presets;
};
}
#endif // PRESETKEYWORDSMODEL_H
