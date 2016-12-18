#ifndef PRESETKEYWORDSMODEL_H
#define PRESETKEYWORDSMODEL_H

#include "../Common/basickeywordsmodel.h"
#include "../Common/baseentity.h"
#include "../Common/abstractlistmodel.h"
#include <QAbstractListModel>
#include <QVector>

namespace Preset {

struct Preset {
   std::shared_ptr<Common::BasicKeywordsModel> keys;
   QString name;
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
        QStringList getKeywords(int index);

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
        Q_INVOKABLE void plainTextEdit(int index, const QString &rawKeywords, bool spaceSeparator);
        Q_INVOKABLE void appendKeyword(int index, const QString &keyword);
        Q_INVOKABLE QObject *getKeywordsModel(int index);
        Q_INVOKABLE void saveToConfig();
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QStringList getFilteredPresets(const QString &  word);
    public slots:
       void presetsUpdated() {LOG_WARNING<<"preset Model"; resetModel();}
    private:
        Common::Hold m_HoldPlaceholder;
        QVector<Preset> m_Presets;
};
}
#endif // PRESETKEYWORDSMODEL_H
