#ifndef UPLOADWATCHER_H
#define UPLOADWATCHER_H

#include <QAbstractListModel>
#include <QVector>
#include <QPair>
#include <QMutex>

namespace Conectivity {
class FtpCoordinator;
}
namespace Conectivity {
    class UploadWatcher: public QAbstractListModel
    {
        Q_OBJECT
    public:
        UploadWatcher( QObject *parent = 0 );
        void injectConnection(Conectivity::FtpCoordinator *ftpCoordinator);
        Q_INVOKABLE QStringList getFailedImages(int index);
    public:
        enum UploadWatcher_Roles {
            HostName = Qt::UserRole + 1
        };
    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
    protected:
        virtual QHash<int, QByteArray> roleNames() const;
    private slots:
        void reportUploadErrorHandler(const QString &filepath, const QString &host, const QString &title);
    private:
        QVector<QPair<QString, QStringList>> m_FtpInfo;
        QMutex m_Mutex;
    };
}
#endif // UPLOADWATCHER_H
