/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "csvexportmodel.h"
#include <QQmlEngine>
#include <QStandardPaths>
#include "csvexportworker.h"
#include <QTimerEvent>

#define MAX_SAVE_PAUSE_RESTARTS 5

namespace MetadataIO {
    CsvExportColumnsModel::CsvExportColumnsModel()
    {
    }

    void CsvExportColumnsModel::setupModel(const std::shared_ptr<CsvExportPlan> &plan) {
        LOG_DEBUG << "#";

        beginResetModel();
        {
            m_ExportPlan = plan;
        }
        endResetModel();
    }

    void CsvExportColumnsModel::clearModel() {
        LOG_DEBUG << "#";

        beginResetModel();
        {
            m_ExportPlan.reset();
        }
        endResetModel();
    }

    void CsvExportColumnsModel::addColumn() {
        LOG_DEBUG << "#";
        if (m_ExportPlan == nullptr) { return; }

        const int size = getColumnsCount();

        beginInsertRows(QModelIndex(), size, size);
        {
            m_ExportPlan->m_PropertiesToExport.emplace_back(Empty, QObject::tr("Untitled"));
        }
        endInsertRows();
    }

    void CsvExportColumnsModel::addColumnAbove(int row) {
        Q_ASSERT(m_ExportPlan != nullptr);
        LOG_INFO << row;
        if (row < 0 || row >= getColumnsCount()) { return; }
        if (m_ExportPlan == nullptr) { return; }

        beginInsertRows(QModelIndex(), row, row);
        {
            CsvExportProperty property;
            property.m_ColumnName = QObject::tr("Untitled");
            property.m_PropertyType = Empty;

            m_ExportPlan->m_PropertiesToExport.insert(m_ExportPlan->m_PropertiesToExport.begin() + row, property);
        }
        endInsertRows();
    }

    void CsvExportColumnsModel::removeColumn(int row) {
        Q_ASSERT(m_ExportPlan != nullptr);
        LOG_INFO << row;
        if (row < 0 || row >= getColumnsCount()) { return; }
        if (m_ExportPlan == nullptr) { return; }

        beginRemoveRows(QModelIndex(), row, row);
        {
            m_ExportPlan->m_PropertiesToExport.erase(m_ExportPlan->m_PropertiesToExport.begin() + row);
        }
        endRemoveRows();
    }

    void CsvExportColumnsModel::moveColumnUp(int row) {
        Q_ASSERT(m_ExportPlan != nullptr);
        LOG_INFO << row;
        if (row < 0 || row >= getColumnsCount()) { return; }
        if (m_ExportPlan == nullptr) { return; }
        if (row == 0) { return; }

        beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
        {
            auto &curr = m_ExportPlan->m_PropertiesToExport[row];
            auto &prev = m_ExportPlan->m_PropertiesToExport[row - 1];
            curr.swap(prev);
        }
        endMoveRows();
    }

    void CsvExportColumnsModel::moveColumnDown(int row) {
        Q_ASSERT(m_ExportPlan != nullptr);
        LOG_INFO << row;
        if (row < 0 || row >= getColumnsCount()) { return; }
        if (m_ExportPlan == nullptr) { return; }
        if (row == getColumnsCount() - 1) { return; }

        // Qt has some incomplications with moving rows down
        // let's just simulate that
        moveColumnUp(row + 1);
    }

    QStringList CsvExportColumnsModel::getPropertiesList() {
        QStringList properties;

        for (int i = 0; i < EXPORT_PROPERTIES_NUMBER; i++) {
            CsvExportPropertyType type = (CsvExportPropertyType)i;
            QString text = csvExportPropertyToString(type);
            properties.append(text);
        }

        return properties;
    }

    int CsvExportColumnsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return getColumnsCount();
    }

    QVariant CsvExportColumnsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= getColumnsCount())) { return QVariant(); }

        auto &property = m_ExportPlan->m_PropertiesToExport[row];

        switch(role) {
        case ColumnNameRole: return property.m_ColumnName;
        case PropertyTypeRole: return (int)property.m_PropertyType;
        default: return QVariant();
        }
    }

    bool CsvExportColumnsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= getColumnsCount()) { return false; }

        auto &property = m_ExportPlan->m_PropertiesToExport[row];

        bool updateRequired = false;

        switch (role) {
        case EditColumnNameRole: {
            auto &columnName = property.m_ColumnName;
            auto newName = value.toString();
            QString sanitized = newName.simplified();

            if (columnName != sanitized) {
                property.m_ColumnName = sanitized;
                updateRequired = true;
            }

            break;
        }
        case EditPropertyRole: {
            const CsvExportPropertyType oldPropertyType = property.m_PropertyType;
            bool ok = false;
            int newPropertyType = value.toInt(&ok);
            if (ok) {
                if (newPropertyType != (int)oldPropertyType) {
                    property.m_PropertyType = (CsvExportPropertyType)newPropertyType;
                    updateRequired = true;

                    if (property.m_ColumnName.trimmed().isEmpty() ||
                            (property.m_ColumnName == "Untitled") ||
                            (property.m_ColumnName == csvExportPropertyToString(oldPropertyType)) ||
                            (property.m_ColumnName == QObject::tr("Untitled"))) {
                        property.m_ColumnName = csvExportPropertyToString(property.m_PropertyType);
                    }
                }
            }
            break;
        }
        default: return false;
        }

        if (updateRequired) {
            emit dataChanged(index, index, QVector<int>() << ColumnNameRole << PropertyTypeRole);
        }

        return true;
    }

    Qt::ItemFlags CsvExportColumnsModel::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || row >= getColumnsCount()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    QHash<int, QByteArray> CsvExportColumnsModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[ColumnNameRole] = "column";
        roles[PropertyTypeRole] = "property";
        roles[EditPropertyRole] = "editproperty";
        roles[EditColumnNameRole] = "editcolumn";
        return roles;
    }

    int CsvExportColumnsModel::getColumnsCount() const {
        if (!m_ExportPlan) { return 0; }
        return (int)m_ExportPlan->m_PropertiesToExport.size();
    }

    /*------------------------------------------------------*/

    CsvExportModel::CsvExportModel():
        m_SaveTimerId(-1),
        m_SaveRestartsCount(0),
        m_IsExporting(false)
    {
        QObject::connect(&m_ExportPlansModel, &CsvExportPlansModel::plansUpdated,
                         this, &CsvExportModel::onPlansUpdated);

        QObject::connect(this, &CsvExportModel::backupRequired, this, &CsvExportModel::onBackupRequired);

        m_ExportDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    }

    void CsvExportModel::setIsExporting(bool value) {
        if (m_IsExporting != value) {
            m_IsExporting = value;
            emit isExportingChanged();
        }
    }

    void CsvExportModel::setOutputDirectory(const QString &value) {
        if (m_ExportDirectory != value) {
            m_ExportDirectory = value;
            emit outputDirectoryChanged();
        }
    }

    void CsvExportModel::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);

        m_ExportPlansModel.setCommandManager(commandManager);
    }

    void CsvExportModel::setupModel(MetadataIO::ArtworksSnapshot::Container &rawSnapshot) {
        m_ArtworksToExport.set(rawSnapshot);
        emit artworksCountChanged();
    }

    void CsvExportModel::initializeExportPlans(Helpers::AsyncCoordinator *initCoordinator) {
        LOG_DEBUG << "#";
        m_ExportPlansModel.initializeConfigs(initCoordinator);
    }

    int CsvExportModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ExportPlans.size();
    }

    QVariant CsvExportModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= (int)m_ExportPlans.size())) { return QVariant(); }

        auto &plan = m_ExportPlans[row];

        switch(role) {
        case PlanNameRole: return plan->m_Name;
        case IsSelectedRole: return plan->m_IsSelected;
        case IsSystemPlanRole: return plan->m_IsSystemPlan;
        default: return QVariant();
        }
    }

    QHash<int, QByteArray> CsvExportModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[PlanNameRole] = "name";
        roles[IsSelectedRole] = "isselected";
        roles[IsSystemPlanRole] = "issystem";
        roles[EditIsSelectedRole] = "editisselected";
        roles[EditPlanNameRole] = "editname";
        return roles;
    }

    bool CsvExportModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= (int)m_ExportPlans.size()) { return false; }

        int roleToUpdate = 0;
        bool updateRequired = false;

        auto &plan = m_ExportPlans[row];

        switch (role)
        {
        case EditIsSelectedRole: {
            bool valueBool = value.toBool();
            if (valueBool != plan->m_IsSelected) {
                plan->m_IsSelected = valueBool;
                updateRequired = true;
                roleToUpdate = IsSelectedRole;
            }
            break;
        }
        case EditPlanNameRole: {
            auto &name = plan->m_Name;
            auto newName = value.toString();
            QString sanitized = newName.simplified();

            if (name != sanitized) {
                plan->m_Name = sanitized;
                updateRequired = true;
                roleToUpdate = PlanNameRole;
            }
            break;
        }
        default:
            return false;
        }

        if (updateRequired) {
            emit dataChanged(index, index, QVector<int>() << roleToUpdate);
        }

        return true;
    }

    Qt::ItemFlags CsvExportModel::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || row >= (int)m_ExportPlans.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    void CsvExportModel::startExport() {
        LOG_DEBUG << "#";
        Q_ASSERT(!m_IsExporting);

        if (retrieveSelectedPlansCount() == 0) {
            LOG_WARNING << "No plans to export";
            return;
        }

        QString exportDirectory = m_ExportDirectory;

        if (exportDirectory.isEmpty()) {
            exportDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        }

        CsvExportWorker *exportWorker = new CsvExportWorker(m_ExportPlans, m_ArtworksToExport, exportDirectory);

        QThread *thread = new QThread();
        exportWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, exportWorker, &CsvExportWorker::process);
        QObject::connect(exportWorker, &CsvExportWorker::stopped, thread, &QThread::quit);

        QObject::connect(exportWorker, &CsvExportWorker::stopped, exportWorker, &CsvExportWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(exportWorker, &CsvExportWorker::stopped, this, &CsvExportModel::onWorkerFinished);

        LOG_DEBUG << "Starting thread...";
        thread->start();

        setIsExporting(true);
    }

    void CsvExportModel::clearModel() {
        LOG_DEBUG << "#";
        m_ArtworksToExport.clear();
    }

    void CsvExportModel::removePlanAt(int row) {
        LOG_INFO << row;

        if (row < 0 || row >= (int)m_ExportPlans.size()) { return; }

        beginRemoveRows(QModelIndex(), row, row);
        {
            m_ExportPlans.erase(m_ExportPlans.begin() + row);
        }
        endRemoveRows();

        justEdited();
    }

    void CsvExportModel::addNewPlan() {
        const int size = (int)m_ExportPlans.size();

        beginInsertRows(QModelIndex(), size, size);
        {
            QString name = QObject::tr("Untitled");
            m_ExportPlans.emplace_back(new CsvExportPlan(name));
        }
        endInsertRows();

        m_ColumnsModel.setupModel(m_ExportPlans.back());

        justEdited();
    }

    QObject *CsvExportModel::getColumnsModel() {
        QObject *object = &m_ColumnsModel;
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
        return object;
    }

    void CsvExportModel::setCurrentItem(int row) {
        LOG_INFO << row;

        if (row < 0 || row >= (int)m_ExportPlans.size()) { return; }

        m_ColumnsModel.setupModel(m_ExportPlans[row]);
    }

    void CsvExportModel::justEdited() {
        if (m_SaveRestartsCount < MAX_SAVE_PAUSE_RESTARTS) {
            if (m_SaveTimerId != -1) {
                this->killTimer(m_SaveTimerId);
                LOG_INTEGR_TESTS_OR_DEBUG << "killed timer" << m_SaveTimerId;
            }

            m_SaveTimerId = this->startTimer(3000, Qt::VeryCoarseTimer);
            LOG_INTEGR_TESTS_OR_DEBUG << "started timer" << m_SaveTimerId;
            m_SaveRestartsCount++;
        } else {
            Q_ASSERT(m_SaveTimerId != -1);
            LOG_INFO << "Maximum backup delays occured, forcing backup";
        }
    }

    void CsvExportModel::saveExportPlans() {
        LOG_DEBUG << "#";
        m_ExportPlansModel.sync(m_ExportPlans);
    }

    int CsvExportModel::retrieveSelectedPlansCount() {
        int count = 0;

        for (auto &plan: m_ExportPlans) {
            if (plan->m_IsSelected) {
                count++;
            }
        }

        return count;
    }

    void CsvExportModel::timerEvent(QTimerEvent *event) {
        if (event == nullptr) { return; }

        LOG_DEBUG << "timer" << event->timerId();

        if (event->timerId() == m_SaveTimerId) {
            m_SaveRestartsCount = 0;
            m_SaveTimerId = -1;

            emit backupRequired();
        }

        // one time event
        this->killTimer(event->timerId());
    }

    void CsvExportModel::onWorkerFinished() {
        LOG_DEBUG << "#";

        setIsExporting(false);
        emit exportFinished();
    }

    void CsvExportModel::onPlansUpdated() {
        LOG_DEBUG << "#";

        auto &updatedPlans = m_ExportPlansModel.getExportPlans();
        beginResetModel();
        {
            m_ExportPlans.swap(updatedPlans);
        }
        endResetModel();

        updatedPlans.clear();
    }

    void CsvExportModel::onBackupRequired() {
        LOG_DEBUG << "#";
        saveExportPlans();
    }
}
