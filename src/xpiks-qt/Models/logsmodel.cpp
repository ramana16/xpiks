/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "logsmodel.h"
#include "../Helpers/loggingworker.h"
#include <QThread>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include "../Helpers/stringhelper.h"
#include "../Helpers/logger.h"
#include "../Helpers/loghighlighter.h"
#include "../Common/defines.h"

namespace Models {

    LogsModel::LogsModel(QObject *parent) :
        QObject(parent),
        m_LoggingWorker(new Helpers::LoggingWorker()),
        m_ColorsModel(nullptr)
    {
#ifdef WITH_LOGS
        m_WithLogs = true;
#else
        m_WithLogs = false;
#endif
    }

    LogsModel::~LogsModel() {
    }

    void LogsModel::startLogging() {
        QThread *loggingThread = new QThread();
        m_LoggingWorker->moveToThread(loggingThread);

        QObject::connect(loggingThread, &QThread::started, m_LoggingWorker, &Helpers::LoggingWorker::process);
        QObject::connect(m_LoggingWorker, &Helpers::LoggingWorker::stopped, loggingThread, &QThread::quit);

        QObject::connect(m_LoggingWorker, &Helpers::LoggingWorker::stopped, m_LoggingWorker, &Helpers::LoggingWorker::deleteLater);
        QObject::connect(loggingThread, &QThread::finished, loggingThread, &QThread::deleteLater);

        loggingThread->start();
    }

    void LogsModel::stopLogging() {
        m_LoggingWorker->cancel();
    }

    void LogsModel::InjectDependency(QMLExtensions::ColorsModel *colorsModel) {
        m_ColorsModel = colorsModel;
    }

    QString LogsModel::getAllLogsText(bool moreLogs) {
        QString result;
#ifdef WITH_LOGS
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        QString logFilePath = logger.getLogFilePath();
        QFile f(logFilePath);

        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // 1000 - do not load the UI
            // advanced users will open logs it notepad
            int numberOfLines = moreLogs ? 1000 : 100;
            QString text = QString::fromUtf8(f.readAll());
            result = Helpers::getLastNLines(text, numberOfLines);
            f.close();
        }
#else
        Q_UNUSED(moreLogs);
        result = QString::fromLatin1("Logs are not available in this version");
#endif
        return result;

    }

    void LogsModel::initLogHighlighting(QQuickTextDocument *document) {
        Q_ASSERT(m_ColorsModel != nullptr);

        if (m_ColorsModel != nullptr) {
            Helpers::LogHighlighter *highlighter = new Helpers::LogHighlighter(m_ColorsModel, document->textDocument());
            Q_UNUSED(highlighter);
        }
    }
}
