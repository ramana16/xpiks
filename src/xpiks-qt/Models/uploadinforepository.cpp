/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "uploadinforepository.h"
#include "uploadinfo.h"
#include <QJsonObject>
#include <QJsonArray>
#include "../Commands/commandmanager.h"
#include "../Encryption/secretsmanager.h"
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"

#define UPLOAD_INFO_SAVE_TIMEOUT 3000
#define UPLOAD_INFO_DELAYS_COUNT 10

#define FTP_DESTINATIONS QLatin1String("destinations")
#define FTP_HOST_KEY QLatin1String("host")
#define FTP_USERNAME_KEY QLatin1String("user")
#define FTP_PASS_KEY QLatin1String("pass")
#define FTP_TITLE_KEY QLatin1String("title")
#define FTP_CREATE_ZIP QLatin1String("zip")
#define FTP_DISABLE_PASSIVE QLatin1String("nopassive")
#define FTP_DISABLE_EPSV QLatin1String("noepsv")
#define FTP_ISSELECTED QLatin1String("selected")

#ifdef QT_DEBUG
    #ifdef INTEGRATION_TESTS
        #define UPLOAD_INFOS_FILE "integration_uploadinfos.json"
    #else
        #define UPLOAD_INFOS_FILE "debug_uploadinfos.json"
    #endif
#else
#define UPLOAD_INFOS_FILE "uploadinfos.json"
#endif

#define OVERWRITE_KEY "overwrite"
#define OVERWRITE_CSV_PLANS false

namespace Models {
    void serializeUploadInfos(const std::vector<std::shared_ptr<UploadInfo> > &uploadInfos, QJsonObject &result) {
        QJsonArray uploadInfoArray;

        for (auto &uploadInfo: uploadInfos) {
            QJsonObject object;

            object.insert(FTP_TITLE_KEY, uploadInfo->getTitle());
            object.insert(FTP_HOST_KEY, uploadInfo->getHost());
            object.insert(FTP_USERNAME_KEY, uploadInfo->getUsername());
            object.insert(FTP_PASS_KEY, uploadInfo->getAnyPassword());
            object.insert(FTP_CREATE_ZIP, uploadInfo->getZipBeforeUpload());
            object.insert(FTP_DISABLE_PASSIVE, uploadInfo->getDisableFtpPassiveMode());
            object.insert(FTP_DISABLE_EPSV, uploadInfo->getDisableEPSV());
            object.insert(FTP_ISSELECTED, uploadInfo->getIsSelected());

            uploadInfoArray.append(object);
        }

        result.insert(FTP_DESTINATIONS, uploadInfoArray);
    }

    bool tryParseHost(const QJsonObject &element, std::shared_ptr<UploadInfo> &uploadInfo) {
        bool parsed = false;

        std::shared_ptr<UploadInfo> destination(new UploadInfo());

        do {
            QJsonValue titleValue = element.value(FTP_TITLE_KEY);
            if (!titleValue.isString()) { break; }
            destination->setTitle(titleValue.toString());

            QJsonValue hostValue = element.value(FTP_HOST_KEY);
            if (!hostValue.isString()) { break; }
            destination->setHost(hostValue.toString());

            QJsonValue usernameValue = element.value(FTP_USERNAME_KEY);
            if (usernameValue.isString()) {
                destination->setUsername(usernameValue.toString());
            }

            QJsonValue passwordValue = element.value(FTP_PASS_KEY);
            if (passwordValue.isString()) {
                destination->setPassword(passwordValue.toString());
            }

            QJsonValue createZipValue = element.value(FTP_CREATE_ZIP);
            if (createZipValue.isBool()) {
                destination->setZipBeforeUpload(createZipValue.toBool(false));
            }

            QJsonValue disablePassiveValue = element.value(FTP_DISABLE_PASSIVE);
            if (disablePassiveValue.isBool()) {
                destination->setDisableFtpPassiveMode(disablePassiveValue.toBool(false));
            }

            QJsonValue disableEpsvValue = element.value(FTP_DISABLE_EPSV);
            if (disableEpsvValue.isBool()) {
                destination->setDisableEPSV(disableEpsvValue.toBool(false));
            }

            QJsonValue isSelectedValue = element.value(FTP_ISSELECTED);
            if (isSelectedValue.isBool()) {
                destination->setIsSelected(isSelectedValue.toBool(false));
            }

            uploadInfo.swap(destination);
            parsed = true;
        } while(false);

        return parsed;
    }

    void parseUploadInfos(const QJsonObject &root, std::vector<std::shared_ptr<UploadInfo> > &uploadInfos) {
        if (!root.contains(FTP_DESTINATIONS)) { return; }
        QJsonValue destinationsObject = root.value(FTP_DESTINATIONS);
        if (!destinationsObject.isArray()) { return; }
        QJsonArray hostsArray = destinationsObject.toArray();
        const int size = hostsArray.size();

        for (int i = 0; i < size; i++) {
            QJsonValue element = hostsArray.at(i);
            if (!element.isObject()) { continue; }

            std::shared_ptr<UploadInfo> item;
            if (tryParseHost(element.toObject(), item)) {
                uploadInfos.emplace_back(item);
            }
        }
    }

    UploadInfoRepository::UploadInfoRepository(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        Common::DelayedActionEntity(UPLOAD_INFO_SAVE_TIMEOUT, UPLOAD_INFO_DELAYS_COUNT),
        m_EmptyPasswordsMode(false)
    {
        QObject::connect(this, &UploadInfoRepository::backupRequired, this, &UploadInfoRepository::onBackupRequired);
    }

    UploadInfoRepository::~UploadInfoRepository() { m_UploadInfos.clear();  }

    void UploadInfoRepository::initFromString(const QString &savedString) {
        LOG_DEBUG << "#";
        if (savedString.isEmpty()) { return; }

        QByteArray originalData;
        originalData.append(savedString.toLatin1());
        QByteArray result = QByteArray::fromBase64(originalData);

        // bad type QList instead of QVector
        // but users already have this
        QList<QHash<int, QString> > items;
        QDataStream stream(&result, QIODevice::ReadOnly);
        stream >> items;

        int length = items.length();
        m_UploadInfos.reserve(length);

        for (int i = 0; i < length; ++i) {
            auto &hash = items.at(i);
            m_UploadInfos.emplace_back(new UploadInfo(hash));
        }

        LOG_INFO << length << "item(s) found";

        justChanged();
    }

    void UploadInfoRepository::initializeConfig() {
        LOG_DEBUG << "#";
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(UPLOAD_INFOS_FILE);
        } else {
            localConfigPath = UPLOAD_INFOS_FILE;
        }

        m_LocalConfig.initConfig(localConfigPath);
        const QJsonDocument &localDocument = m_LocalConfig.getConfig();

        decltype(m_UploadInfos) tempInfos;
        parseUploadInfos(localDocument.object(), tempInfos);
        LOG_INFO << "Parsed" << tempInfos.size() << "upload host(s)";

        if (!tempInfos.empty()) {
            m_UploadInfos.swap(tempInfos);
        }
    }

    void UploadInfoRepository::removeItem(int row) {
        beginRemoveRows(QModelIndex(), row, row);
        {
            removeInnerItem(row);
        }
        endRemoveRows();
        emit infosCountChanged();

        justChanged();
    }

    void UploadInfoRepository::addItem() {
        int lastIndex = (int)m_UploadInfos.size();

        LOG_INFO << lastIndex;
        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        {
            m_UploadInfos.emplace_back(new UploadInfo());
        }
        endInsertRows();

        emit infosCountChanged();

        justChanged();
    }

    int UploadInfoRepository::getSelectedInfosCount() const {
        int selectedCount = 0;

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    QString UploadInfoRepository::getAgenciesWithMissingDetails() {
        QStringList items;

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected() && info->isSomethingMissing()) {
                items.append(info->getTitle());
            }
        }

        return items.join(", ");
    }

    void UploadInfoRepository::updateProperties(int itemIndex) {
        QModelIndex itemModelIndex = index(itemIndex);
        // could be any role - just to triger Advanced tab checkbox connections
        emit dataChanged(itemModelIndex, itemModelIndex, QVector<int>() << ZipBeforeUploadRole);
    }

    // mp == master password
    void UploadInfoRepository::initializeAccounts(bool mpIsCorrectOrEmpty) {
        this->setEmptyPasswordsMode(!mpIsCorrectOrEmpty);
        if (!mpIsCorrectOrEmpty) {
            this->backupAndDropRealPasswords();
        }
    }

    void UploadInfoRepository::finalizeAccounts() {
        if (m_EmptyPasswordsMode) {
            this->restoreRealPasswords();
        }
    }

    void UploadInfoRepository::backupAndDropRealPasswords() {
        for (auto &info: m_UploadInfos) {
            info->backupPassword();
            info->dropPassword();
        }
    }

    void UploadInfoRepository::restoreRealPasswords() {
        for (auto &info: m_UploadInfos) {
            info->restorePassword();
        }
    }

    void UploadInfoRepository::updatePercentages() {
        emit dataChanged(index(0), index((int)m_UploadInfos.size() - 1), QVector<int>() << PercentRole);
    }

    void UploadInfoRepository::resetPercents() {
        LOG_DEBUG << "#";
        for (auto &info: m_UploadInfos) {
            info->resetPercent();
        }
    }

    std::vector<std::shared_ptr<UploadInfo> > UploadInfoRepository::retrieveSelectedUploadInfos() const {
        std::vector<std::shared_ptr<UploadInfo> > uploadInfos;
        uploadInfos.reserve(m_UploadInfos.size());

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected()) {
                uploadInfos.push_back(info);
            }
        }

        return uploadInfos;
    }

    int UploadInfoRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_UploadInfos.size();
    }

    QVariant UploadInfoRepository::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return QVariant();
        }

        auto &uploadInfo = m_UploadInfos.at(row);

        switch (role) {
            case TitleRole:
                return uploadInfo->getTitle();
            case HostRole:
                return uploadInfo->getHost();
            case UsernameRole:
                return uploadInfo->getUsername();
            case PasswordRole: {
                const QString &encodedPassword = uploadInfo->getPassword();
                QString password = m_CommandManager->getSecretsManager()->decodePassword(encodedPassword);
                return password;
            }
            case IsSelectedRole:
                return uploadInfo->getIsSelected();
            case ZipBeforeUploadRole:
                return uploadInfo->getZipBeforeUpload();
            case PercentRole: {
            double percent = uploadInfo->getPercent();
            return ((0.0001 < percent) && (percent < 1.0)) ? 1.0 : percent;
        }
            /*case FtpPassiveModeRole:
                return uploadInfo->getFtpPassiveMode();*/
            case DisableFtpPassiveModeRole:
                return uploadInfo->getDisableFtpPassiveMode();
            case DisableEPSVRole:
                return uploadInfo->getDisableEPSV();
            default:
                return QVariant();
        }
    }

    Qt::ItemFlags UploadInfoRepository::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool UploadInfoRepository::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return false;
        }

        auto &uploadInfo = m_UploadInfos.at(row);
        QString title;
        int roleToUpdate = 0;
        bool needToUpdate = false;

        switch (role) {
            case EditTitleRole:
                roleToUpdate = TitleRole;
                title = value.toString();
                needToUpdate = uploadInfo->setTitle(title);
                // hack for updating checkbox binding
                if (!needToUpdate) {
                    needToUpdate = true;
                    roleToUpdate = ZipBeforeUploadRole;
                }

                break;
            case EditHostRole:
                roleToUpdate = HostRole;
                needToUpdate = uploadInfo->setHost(value.toString().simplified());
                break;
            case EditUsernameRole:
                roleToUpdate = UsernameRole;
                needToUpdate = uploadInfo->setUsername(value.toString().simplified());
                break;
            case EditPasswordRole: {
                roleToUpdate = PasswordRole;
                QString rawPassword = value.toString();
                QString encodedPassword = m_CommandManager->getSecretsManager()->encodePassword(rawPassword);
                // skip needUpdate
                uploadInfo->setPassword(encodedPassword);
                break;
            }
            case EditIsSelectedRole:
                roleToUpdate = IsSelectedRole;
                needToUpdate = uploadInfo->setIsSelected(value.toBool());
                break;
            case EditZipBeforeUploadRole:
                roleToUpdate = ZipBeforeUploadRole;
                needToUpdate = uploadInfo->setZipBeforeUpload(value.toBool());
                break;
            /*case EditFtpPassiveModeRole:
                roleToUpdate = FtpPassiveModeRole;
                needToUpdate = uploadInfo->setFtpPassiveMode(value.toBool());*/
            case EditDisableFtpPassiveModeRole:
                roleToUpdate = DisableFtpPassiveModeRole;
                needToUpdate = uploadInfo->setDisableFtpPassiveMode(value.toBool());
                break;
            case EditDisableEPSVRole:
                roleToUpdate = DisableEPSVRole;
                needToUpdate = uploadInfo->setDisableEPSV(value.toBool());
                break;
            default:
                return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << roleToUpdate);
            justChanged();
        }

        return true;
    }

    void UploadInfoRepository::onBeforeMasterPasswordChanged(const QString &oldMasterPassword,
                                                             const QString &newMasterPassword) {
        LOG_INFO << "#";
        xpiks()->recodePasswords(oldMasterPassword, newMasterPassword, m_UploadInfos);
    }

    void UploadInfoRepository::onAfterMasterPasswordReset() {
        LOG_INFO << "#";
        for (auto &info: m_UploadInfos) {
            info->dropPassword();
        }
    }

    void UploadInfoRepository::onBackupRequired() {
        LOG_DEBUG << "#";

        if (saveUploadInfos()) {
            auto *settingsModel = m_CommandManager->getSettingsModel();
            settingsModel->clearLegacyUploadInfos();
        }
    }

    bool UploadInfoRepository::saveUploadInfos() {
        LOG_DEBUG << "#";
        QJsonObject uploadInfosObject;
        serializeUploadInfos(m_UploadInfos, uploadInfosObject);

        QJsonDocument doc;
        doc.setObject(uploadInfosObject);

        Helpers::LocalConfigDropper dropper(&m_LocalConfig);
        Q_UNUSED(dropper);

        m_LocalConfig.setConfig(doc);
        bool success = m_LocalConfig.saveToFile();
        return success;
    }

    QHash<int, QByteArray> UploadInfoRepository::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[HostRole] = "host";
        roles[UsernameRole] = "username";
        roles[PasswordRole] = "password";
        roles[EditTitleRole] = "edittitle";
        roles[EditHostRole] = "edithost";
        roles[EditUsernameRole] = "editusername";
        roles[EditPasswordRole] = "editpassword";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[ZipBeforeUploadRole] = "zipbeforeupload";
        roles[EditZipBeforeUploadRole] = "editzipbeforeupload";
        roles[PercentRole] = "percent";
        /*roles[FtpPassiveModeRole] = "ftppassivemode";
           roles[EditFtpPassiveModeRole] = "editftppassivemode";*/
        roles[DisableFtpPassiveModeRole] = "disablepassivemode";
        roles[EditDisableFtpPassiveModeRole] = "editdisablepassivemode";
        roles[DisableEPSVRole] = "disableEPSV";
        roles[EditDisableEPSVRole] = "editdisableEPSV";
        return roles;
    }

    void UploadInfoRepository::removeInnerItem(int row) {
        m_UploadInfos.erase(m_UploadInfos.begin() + row);
    }
}
