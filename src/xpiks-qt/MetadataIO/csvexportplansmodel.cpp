/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "csvexportplansmodel.h"
#include <QDir>
#include <QThread>
#include <QJsonArray>
#include <QJsonDocument>
#include "../Helpers/asynccoordinator.h"
#include "../Connectivity/apimanager.h"

#define PLAN_NAME_KEY QLatin1String("name")
#define PLAN_IS_SYSTEM_KEY QLatin1String("issystem")
#define PLAN_PROPERTIES_KEY QLatin1String("properties")
#define PLANS_KEY QLatin1String("plans")
#define PROPERTY_TYPE_KEY QLatin1String("propertyType")
#define PROPERTY_NAME_KEY QLatin1String("propertyName")
#define COLUMN_NAME_KEY QLatin1String("columnName")

#ifdef QT_DEBUG
    #ifdef INTEGRATION_TESTS
        #define EXPORT_PLANS_FILE "integration_csv_export_plans.json"
    #else
        #define EXPORT_PLANS_FILE "debug_csv_export_plans.json"
    #endif
#else
#define EXPORT_PLANS_FILE "csv_export_plans.json"
#endif

#define OVERWRITE_KEY "overwrite"
#define OVERWRITE_CSV_PLANS false

namespace MetadataIO {
    void propertiesToJsonArray(QJsonArray &propertiesArray, const std::vector<CsvExportProperty> &propertiesToExport) {
        for (auto &property: propertiesToExport) {
            QJsonObject propertyObject;

            const int propertyType = (int)property.m_PropertyType;
            propertyObject.insert(PROPERTY_TYPE_KEY, propertyType);
            propertyObject.insert(COLUMN_NAME_KEY, property.m_ColumnName);
            QLatin1String propertyName = csvExportPropertyToString(property.m_PropertyType);
            propertyObject.insert(PROPERTY_NAME_KEY, propertyName);

            propertiesArray.append(propertyObject);
        }
    }

    void jsonArrayToProperties(std::vector<CsvExportProperty> &propertiesToExport, const QJsonArray &propertiesArray) {
        const int size = propertiesArray.size();
        propertiesToExport.resize(size);

        for (int i = 0; i < size; ++i) {
            QJsonValue element = propertiesArray.at(i);
            if (!element.isObject()) {
                LOG_WARNING << "Element is not an object at" << i;
                continue;
            }

            QJsonObject propertyObject = element.toObject();

            QJsonValue propIdValue = propertyObject.value(PROPERTY_TYPE_KEY);
            if (!propIdValue.isDouble()) {
                LOG_WARNING << "Property ID is not a number:" << propIdValue.toString();
                continue;
            }

            QJsonValue columnNameValue = propertyObject.value(COLUMN_NAME_KEY);
            QString columnName = QT_TRANSLATE_NOOP("CsvExport", "Untitled");
            if (!columnNameValue.isString()) {
                LOG_WARNING << "Column name is not a string" << columnNameValue.toString();
            } else {
                QString nameFromJson = columnNameValue.toString();
                if (!nameFromJson.trimmed().isEmpty()) {
                    columnName = nameFromJson;
                }
            }

            propertiesToExport[i].m_ColumnName = columnName;

            const int propertyType = propIdValue.toInt();

            QJsonValue propNameValue = propertyObject.value(PROPERTY_NAME_KEY);
            if (!propNameValue.isString()) {
                LOG_WARNING << "Property name is not a string:" << propNameValue.toString();
            }

            QString propertyName = propNameValue.toString();
            QString derivedPropertyName = csvExportPropertyToString((CsvExportPropertyType)propertyType);
            Q_ASSERT(derivedPropertyName == propertyName);
            if (derivedPropertyName != propertyName) {
                LOG_WARNING << "CSV property does not match at" << i;
            }

            if ((0 <= propertyType) && (propertyType < (int)EXPORT_PROPERTIES_NUMBER)) {
                propertiesToExport[i].m_PropertyType = (CsvExportPropertyType)propertyType;
            } else {
                LOG_WARNING << "CSV property is unknown:" << propertyType << "at" << i;
                propertiesToExport[i].m_PropertyType = Empty;
            }
        }
    }

    void deserializeExportPlansArray(std::vector<std::shared_ptr<CsvExportPlan> > &exportPlans, const QJsonArray &plansArray) {
        const int size = plansArray.size();

        exportPlans.clear();
        exportPlans.resize(size);

        for (int i = 0; i < size; ++i) {
            QJsonValue element = plansArray.at(i);
            if (!element.isObject()) {
                LOG_WARNING << "Element is not an object at" << i;
                continue;
            }

            QJsonObject planPropertiesObject = element.toObject();
            auto &exportPlan = exportPlans[i];

            QJsonValue nameValue = planPropertiesObject.value(PLAN_NAME_KEY);
            if (!nameValue.isString()) {
                LOG_WARNING << "Element at" << i << "does not have a valid name";
                continue;
            }

            exportPlan.reset(new CsvExportPlan());

            exportPlan->m_Name = nameValue.toString();

            QJsonValue isSystemValue = planPropertiesObject.value(PLAN_IS_SYSTEM_KEY);
            if (isSystemValue.isBool()) {
                exportPlan->m_IsSystemPlan = isSystemValue.toBool(false);
            }

            QJsonValue propertiesValue = planPropertiesObject.value(PLAN_PROPERTIES_KEY);
            if (!propertiesValue.isArray()) {
                LOG_WARNING << "Element at" << i << "does not have a valid properties list";
                continue;
            }

            QJsonArray propertiesArray = propertiesValue.toArray();
            jsonArrayToProperties(exportPlan->m_PropertiesToExport, propertiesArray);
        }
    }

    QJsonObject serializeExportPlans(const std::vector<std::shared_ptr<CsvExportPlan> > &exportPlans) {
        QJsonArray plansArray;

        for (auto &plan: exportPlans) {
            if ((plan->m_Name == "Untitled") && plan->m_PropertiesToExport.empty()) {
                continue;
            }

            QJsonObject planObject;
            planObject.insert(PLAN_NAME_KEY, plan->m_Name);

            planObject.insert(PLAN_IS_SYSTEM_KEY, plan->m_IsSystemPlan);

            QJsonArray propertiesArray;
            propertiesToJsonArray(propertiesArray, plan->m_PropertiesToExport);
            planObject.insert(PLAN_PROPERTIES_KEY, propertiesArray);

            plansArray.append(planObject);
        }

        QJsonObject result;
        result.insert(PLANS_KEY, plansArray);
        return result;
    }

    CsvExportPlansModel::CsvExportPlansModel(QObject *parent):
        Models::AbstractConfigUpdaterModel(OVERWRITE_CSV_PLANS, parent)
    {
    }

    void CsvExportPlansModel::initializeConfigs(Helpers::AsyncCoordinator *initCoordinator) {
        LOG_DEBUG << "#";

        Helpers::AsyncCoordinatorLocker locker(initCoordinator);
        Helpers::AsyncCoordinatorUnlocker unlocker(initCoordinator);
        Q_UNUSED(locker); Q_UNUSED(unlocker);

        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(EXPORT_PLANS_FILE);
        } else {
            localConfigPath = EXPORT_PLANS_FILE;
        }

        auto &apiManager = Connectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getCsvExportPlansAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);

        emit plansUpdated();
    }

    void CsvExportPlansModel::sync(const std::vector<std::shared_ptr<CsvExportPlan> > &exportPlans) {
        LOG_INFO << exportPlans.size() << "plan(s)";

        QJsonObject plansObject = serializeExportPlans(exportPlans);

        QJsonDocument doc;
        doc.setObject(plansObject);

        Helpers::LocalConfig &localConfig = getLocalConfig();

        Helpers::LocalConfigDropper dropper(&localConfig);
        Q_UNUSED(dropper);

        localConfig.setConfig(doc);
        localConfig.saveToFile();
    }

    bool CsvExportPlansModel::processLocalConfig(const QJsonDocument &document) {
        bool anyError = false;

        if (document.isObject()) {
            QJsonObject exportPlansObject = document.object();
            deserializeExportPlans(exportPlansObject);
        } else {
            LOG_WARNING << "JSON document doesn't contain an object";
        }

        return anyError;
    }

    void CsvExportPlansModel::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        bool overwrite = false;

        if (!overwriteLocal && remoteDocument.isObject()) {
            QJsonObject rootObject = remoteDocument.object();
            if (rootObject.contains(OVERWRITE_KEY)) {
                QJsonValue overwriteValue = rootObject[OVERWRITE_KEY];
                if (overwriteValue.isBool()) {
                    overwrite = overwriteValue.toBool();
                    LOG_DEBUG << "Overwrite flag present in the config:" << overwrite;
                } else {
                    LOG_WARNING << "Overwrite flag is not boolean";
                }
            }
        } else {
            overwrite = overwriteLocal;
        }

        Models::AbstractConfigUpdaterModel::processRemoteConfig(remoteDocument, overwrite);
    }

    void CsvExportPlansModel::processMergedConfig(const QJsonDocument &document) {
        LOG_DEBUG << "#";

        if (document.isObject()) {
            m_ExportPlans.clear();

            QJsonObject exportPlansObject = document.object();
            deserializeExportPlans(exportPlansObject);

            emit plansUpdated();
        } else {
            LOG_WARNING << "JSON document doesn't contain an object";
        }
    }

    int CsvExportPlansModel::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        if (val1.contains(PLAN_NAME_KEY) && val2.contains(PLAN_NAME_KEY)) {
            const QString planName1 = val1.value(PLAN_NAME_KEY).toString();
            const QString planName2 = val2.value(PLAN_NAME_KEY).toString();
            int result = QString::compare(planName1, planName2);
            return result;
        } else if (val1.contains(PROPERTY_TYPE_KEY) && val2.contains(PROPERTY_TYPE_KEY)) {
            const QString columnName1 = val1.value(COLUMN_NAME_KEY).toString();
            const QString columnName2 = val2.value(COLUMN_NAME_KEY).toString();

            const int type1 = val1.value(PROPERTY_TYPE_KEY).toInt(-1);
            const int type2 = val2.value(PROPERTY_TYPE_KEY).toInt(-1);

            if ((type1 != -1) && (type1 == type2) && (columnName1 == columnName2)) {
                return 0;
            }
        }

        return 1;
    }

    void CsvExportPlansModel::deserializeExportPlans(const QJsonObject &object) {
        Q_ASSERT(m_ExportPlans.empty());

        QJsonValue plansValue = object.value(PLANS_KEY);
        if (!plansValue.isArray()) {
            LOG_WARNING << "Object does not contain any plans. 'plans' type is" << plansValue.type();
            return;
        }

        QJsonArray plansArray = plansValue.toArray();
        std::vector<std::shared_ptr<CsvExportPlan> > exportPlans;

        deserializeExportPlansArray(exportPlans, plansArray);

        exportPlans.erase(
                    std::remove_if(exportPlans.begin(), exportPlans.end(),
                                   [](const std::shared_ptr<CsvExportPlan> &plan) {
            return (!plan) || (plan->m_Name.isEmpty()) || (plan->m_PropertiesToExport.empty()); }),
                    exportPlans.end());

        LOG_INFO << "Parsed" << exportPlans.size() << "export plan(s)";

        m_ExportPlans.swap(exportPlans);
    }
}
