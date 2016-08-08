#include "uploadwatcher.h"
#include <QFileInfo>

namespace Conectivity {
    UploadWatcher::UploadWatcher(QObject *parent):
        QAbstractListModel(parent)
    {}

    void UploadWatcher::resetModel() {
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
            case FtpAddress:
                return item.first;
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> UploadWatcher::roleNames() const {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();
        names[FtpAddress] = "ftpaddress";
        return names;
    }

    void UploadWatcher::reportUploadErrorHandler(const QString &filepath, const QString &host) {
        QFileInfo fi(filepath);

        if (fi.suffix() == "eps") {
            return;
        }

        bool found = false;
        int size = m_FtpInfo.size();

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
