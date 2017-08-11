/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOGSMODEL
#define LOGSMODEL

#include <QObject>
#include <QQuickTextDocument>

namespace Helpers {
    class LoggingWorker;
}

namespace QMLExtensions {
    class ColorsModel;
}

class QString;

namespace Models {
    class LogsModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool withLogs READ getWithLogs CONSTANT)

    public:
        LogsModel(QObject *parent=NULL);
        virtual ~LogsModel();

    public:
        void startLogging();
        void stopLogging();
        void InjectDependency(QMLExtensions::ColorsModel *colorsModel);

    public:
        Q_INVOKABLE QString getAllLogsText(bool moreLogs=false);
        Q_INVOKABLE void initLogHighlighting(QQuickTextDocument *document);
        bool getWithLogs() const { return m_WithLogs; }

    private:
        Helpers::LoggingWorker *m_LoggingWorker;
        QMLExtensions::ColorsModel *m_ColorsModel;
        bool m_WithLogs;
    };
}

#endif // LOGSMODEL

