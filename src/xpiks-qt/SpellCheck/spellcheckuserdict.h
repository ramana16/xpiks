#include "../Common/abstractlistmodel.h"
#include <QSet>
#include <QReadWriteLock>
#include "../Common/flags.h"
#include "../Common/baseentity.h"

#ifndef SPELLCHECKUSERDICT_H
#define SPELLCHECKUSERDICT_H

namespace SpellCheck {
    class SpellCheckUserDict:
        public Common::AbstractListModel,
        public Common::BaseEntity
    {
    Q_OBJECT
    Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)

    public:
        SpellCheckUserDict(QObject *parent=0);

        ~SpellCheckUserDict() = default;

    public:
        void loadModelFromFile();

    public:
        enum BasicKeywordsModel_Roles {
            KeywordRole = Qt::UserRole + 1
        };

    public:
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges);

    protected:
        virtual void removeInnerItem(int row);

    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        int getKeywordsCount();

    public:
        Q_INVOKABLE QString removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void saveToExternalModel();

    public slots:
        void userDictUpdateHandler(const QStringList &keywords, bool overwritten) {if (!overwritten) appendKeywords(keywords); }
        void userDictClearedHandler() {clearModel(); }
   signals:
        void keywordsCountChanged();

    public:
        bool removeKeywordAt(int index, QString &removedKeyword);
        bool removeLastKeyword(QString &removedKeyword);
        int appendKeywords(const QStringList &keywordsList);

    private:
        bool areKeywordsEmpty();
        void doTakeKeyword(int index, QString &removedKeyword);
        bool doEditKeyword(int index, const QString &replacement);
        bool doClearKeywords();
        bool canBeAdded(const QString &keyword) const;

    public:
        virtual bool isEmpty();

    public:
        Q_INVOKABLE bool hasKeyword(const QString &keyword);
        Q_INVOKABLE bool canEditKeyword(int index, const QString &replacement);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QStringList m_KeywordsList;
        QSet<QString> m_KeywordsSet;
    };
}
#endif // SPELLCHECKUSERDICT_H
