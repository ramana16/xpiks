#include "uploadwatcher.h"
#include "Conectivity/ftpcoordinator.h"
#include <QFileInfo>

namespace Conectivity {
    UploadWatcher::UploadWatcher(QObject *parent):
        QAbstractListModel(parent)
    {}


    void UploadWatcher::resetModel()
    {
        beginResetModel();
        m_FtpInfo.clear();
        endResetModel();
    }

    QStringList UploadWatcher::getFailedImages(int row) {
        if (row < 0 || row >= (int)m_FtpInfo.size()) {
            return QStringList();
        }
        auto &item = m_FtpInfo.at(row);
        return item.second;
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

    void UploadWatcher::reportUploadErrorHandler(const QString &filepath, const QString &host) {
        QMutexLocker lock(&m_Mutex);

        bool found = false;
        int size = m_FtpInfo.size();
        if (filepath.right(3).toLower() == "eps"){
            return;
        }
        for (int i = 0; i < size; i++) {
            if (m_FtpInfo[i].first == host) {
                m_FtpInfo[i].second.append(filepath);
                found = true;
                break;
            }
        }
        if (!found) {
            m_FtpInfo.append(QPair<QString, QStringList>(host, QStringList(filepath)));
        }
    }
}
