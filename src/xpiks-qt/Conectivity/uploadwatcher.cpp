#include "uploadwatcher.h"
#include <QFileInfo>
#include "../Common/defines.h"

namespace Conectivity {
    UploadWatcher::UploadWatcher(QObject *parent):
        QAbstractListModel(parent),
        m_failedImagesCount(0)
    {}

    void UploadWatcher::resetModel() {
        LOG_INFO << "Resetting UploadWatcher..";
        beginResetModel();
        m_ftpInfo.clear();
        m_failedImagesCount = 0;
        endResetModel();
        emit failedImagesCountChanged();
    }

    QStringList UploadWatcher::getFailedImages(int row) {
        if (row < 0 || row >= (int)m_ftpInfo.size()) {
            return QStringList();
        }

        auto &item = m_ftpInfo.at(row);
        return item.second;
    }

    int UploadWatcher::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ftpInfo.size();
    }

    QVariant UploadWatcher::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= (int)m_ftpInfo.size()) {
            return QVariant();
        }

        auto &item = m_ftpInfo.at(row);

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
        LOG_WARNING << "Upload failed for file [" << filepath << "] to host {" << host << "}";
        if (QFileInfo(filepath).completeSuffix().toLower() == "eps") {
            LOG_INFO << "Skipping" << filepath;
            return;
        }

        bool found = false;
        int size = m_ftpInfo.size();

        for (int i = 0; i < size; i++) {
            if (m_ftpInfo[i].first == host) {
                m_ftpInfo[i].second.append(filepath);
                found = true;
                break;
            }
        }

        if (!found) {
            m_ftpInfo.append(QPair<QString, QStringList>(host, QStringList(filepath)));
            LOG_INFO << "Creating new entry for" << host;
        }

        m_failedImagesCount++;
        emit failedImagesCountChanged();
    }
}
