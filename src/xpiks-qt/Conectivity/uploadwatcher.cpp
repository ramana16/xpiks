#include "uploadwatcher.h"
#include "Conectivity/ftpcoordinator.h"

namespace Conectivity {
    UploadWatcher::UploadWatcher(QObject *parent):
        QAbstractListModel(parent)
    {}

    void UploadWatcher::injectConnection(FtpCoordinator *ftpCoordinator) {
        QObject::connect(ftpCoordinator, SIGNAL(transferFailedSignal(QString, QString, QString)),
                         this, SLOT(reportUploadErrorHandler( QString, QString, QString) ) );
    }

    QStringList UploadWatcher::getFailedImages(int index) {
        return m_FtpInfo.at(index).second;
    }

    int UploadWatcher::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_FtpInfo.size();
    }

    QVariant UploadWatcher::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= (int)m_FtpInfo.size()) {
            return QVariant();
        }

        auto &item = m_FtpInfo.at(row);

        switch (role) {
            case HostName:
                return item.first;
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> UploadWatcher::roleNames() const {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();
        names[HostName] = "hostname";
        return names;
    }

    void UploadWatcher::reportUploadErrorHandler(const QString &filepath, const QString &host, const QString &title) {
        Q_UNUSED(host);
        QMutexLocker lock(&m_Mutex);

        LOG_INFO<<"OKI DOKI S"<<title<<" "<<filepath;
        bool found = false;
        int size = m_FtpInfo.size();

        for (int i = 0; i < size; i++) {
            if (m_FtpInfo[i].first == title) {
                m_FtpInfo[i].second.append(filepath);
                found = true;
                break;
            }
        }

        if (!found) {
            m_FtpInfo.append(QPair<QString, QStringList>(title, QStringList(filepath)));
        }
    }
}
