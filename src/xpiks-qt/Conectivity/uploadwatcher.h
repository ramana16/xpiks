#ifndef UPLOADWATCHER_H
#define UPLOADWATCHER_H

#include <QAbstractListModel>
#include <QVector>
#include <QPair>

namespace Conectivity {
    class UploadWatcher:
        public QAbstractListModel
    {
    Q_OBJECT

    public:
        UploadWatcher(QObject *parent=0);
        void resetModel();
        Q_INVOKABLE QStringList getFailedImages(int row);

    public:
        enum UploadWatcher_Roles {
            FtpAddress = Qt::UserRole + 1
        };

    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;

    public slots:
        void reportUploadErrorHandler(const QString &filepath, const QString &host);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QVector<QPair<QString, QStringList> > m_FtpInfo;
    };
}
#endif // UPLOADWATCHER_H
