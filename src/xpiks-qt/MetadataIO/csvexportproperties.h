/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CSVEXPORTPROPERTIES_H
#define CSVEXPORTPROPERTIES_H

#include <QString>
#include <vector>

namespace MetadataIO {
    enum CsvExportPropertyType {
        Empty = 0,
        Filename = 1,
        Title = 2,
        Description = 3,
        Keywords = 4,
        Category1 = 5,
        Category2 = 6,
        EXPORT_PROPERTIES_NUMBER
    };

    QLatin1String csvExportPropertyToString(CsvExportPropertyType property);

    struct CsvExportProperty {
        CsvExportProperty():
            m_PropertyType(Empty),
            m_ColumnName("Untitled")
        { }

        CsvExportProperty(CsvExportPropertyType propertyType):
            m_PropertyType(propertyType)
        {
            m_ColumnName = csvExportPropertyToString(m_PropertyType);
        }

        CsvExportProperty(CsvExportPropertyType propertyType, const QString &columnName):
            m_PropertyType(propertyType),
            m_ColumnName(columnName)
        { }

        void swap(CsvExportProperty &other) {
            m_ColumnName.swap(other.m_ColumnName);

            CsvExportPropertyType temp = m_PropertyType;
            m_PropertyType = other.m_PropertyType;
            other.m_PropertyType = temp;
        }

        CsvExportPropertyType m_PropertyType;
        QString m_ColumnName;
    };

    struct CsvExportPlan {
        CsvExportPlan():
            m_IsSelected(false),
            m_IsSystemPlan(false)
        {
        }

        CsvExportPlan(const QString &name):
            m_Name(name),
            m_IsSelected(false),
            m_IsSystemPlan(false)
        {
        }

        std::vector<CsvExportProperty> m_PropertiesToExport;
        QString m_Name;
        bool m_IsSelected;
        bool m_IsSystemPlan;
    };
}

#endif // CSVEXPORTPROPERTIES_H
