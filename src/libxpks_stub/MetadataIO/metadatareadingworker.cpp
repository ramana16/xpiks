/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadatareadingworker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QImageReader>
#include <Models/settingsmodel.h>
#include <Models/artworkmetadata.h>
#include <Helpers/asynccoordinator.h>
#include <MetadataIO/metadatareadinghub.h>
#include <Helpers/constants.h>
#include <Common/defines.h>

#ifdef Q_OS_WIN
#define _X86_
#include <fileapi.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define SOURCEFILE QLatin1String("SourceFile")
#define TITLE QLatin1String("Title")
#define DESCRIPTION QLatin1String("Description")
#define OBJECTNAME QLatin1String("ObjectName")
#define IMAGEDESCRIPTION QLatin1String("ImageDescription")
#define CAPTIONABSTRACT QLatin1String("Caption-Abstract")
#define KEYWORDS QLatin1String("Keywords")
#define SUBJECT QLatin1String("Subject")
#define DATETAKEN QLatin1String("DateTimeOriginal")
#define TIMEZONE QLatin1String("TimeZoneOffset")
#define IMAGEWIDTH QLatin1String("ImageWidth")
#define IMAGEHEIGHT QLatin1String("ImageHeight")

namespace libxpks {
    namespace io {
        void parseJsonKeywords(const QJsonArray &array, MetadataIO::OriginalMetadata *result) {
            int size = array.size();
            QStringList keywords;
            keywords.reserve(size);

            for (int i = 0; i < size; ++i) {
                keywords.append(array.at(i).toString());
            }

            result->m_Keywords = keywords;
        }

        bool parseStringKeywords(QString &keywords, MetadataIO::OriginalMetadata *result) {
            bool parsed = false;
            if (!keywords.isEmpty()) {
                // old Xpiks bug when it called exiftool with wrong arguments
                if (keywords.startsWith(QChar('"')) && keywords.endsWith('"')) {
                    keywords = keywords.mid(1, keywords.length() - 2);
                }

                result->m_Keywords = keywords.split(QChar(','), QString::SkipEmptyParts);
                parsed = true;
            }

            return parsed;
        }

        int parseTimeZoneOffset(const QJsonObject &object) {
            // NOT the standard tag
            // from the spec it contains 1 or 2 values
            QJsonValue timeZone = object.value(TIMEZONE);
            int timeZoneOffset;
            if (timeZone.isArray()) {
                QJsonArray timeZoneArray = timeZone.toArray();
                timeZoneOffset = timeZoneArray.at(0).toInt();
            } else {
                timeZoneOffset = timeZone.toInt();
            }

            return timeZoneOffset;
        }

        void jsonObjectToImportResult(const QJsonObject &object, MetadataIO::OriginalMetadata *result) {
            if (object.contains(SOURCEFILE)) {
                result->m_FilePath = object.value(SOURCEFILE).toString();
            }

            if (object.contains(TITLE)) {
                result->m_Title = object.value(TITLE).toString();
            } else if (object.contains(OBJECTNAME)) {
                result->m_Title = object.value(OBJECTNAME).toString();
            }

            if (object.contains(DESCRIPTION)) {
                result->m_Description = object.value(DESCRIPTION).toString();
            } else if (object.contains(CAPTIONABSTRACT)) {
                result->m_Description = object.value(CAPTIONABSTRACT).toString();
            } else if (object.contains(IMAGEDESCRIPTION)) {
                result->m_Description = object.value(IMAGEDESCRIPTION).toString();
            }

            if (object.contains(DATETAKEN)) {
                QString dateTime = object.value(DATETAKEN).toString();
                result->m_DateTimeOriginal = QDateTime::fromString(dateTime, Qt::ISODate);
            }

            bool keywordsSet = false;

            if (object.contains(KEYWORDS)) {
                QJsonValue keywords = object.value(KEYWORDS);
                if (keywords.isArray()) {
                    parseJsonKeywords(keywords.toArray(), result);
                    keywordsSet = true;
                } else {
                    LOG_WARNING << "Keywords object in json is not array";
                    QString keywordsStr = object.value(KEYWORDS).toString().trimmed();
                    keywordsSet = parseStringKeywords(keywordsStr, result);
                }
            }

            if (!keywordsSet && object.contains(SUBJECT)) {
                QJsonValue subject = object.value(SUBJECT);
                if (subject.isArray()) {
                    parseJsonKeywords(subject.toArray(), result);
                } else {
                    QString keywordsStr = object.value(SUBJECT).toString().trimmed();
                    parseStringKeywords(keywordsStr, result);
                }
            }

            if (object.contains(IMAGEWIDTH)) {
                result->m_ImageSize.setWidth((int)object.value(IMAGEWIDTH).toDouble(0));
            }

            if (object.contains(IMAGEHEIGHT)) {
                result->m_ImageSize.setWidth((int)object.value(IMAGEHEIGHT).toDouble(0));
            }
        }

        ExiftoolImageReadingWorker::ExiftoolImageReadingWorker(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                                               Models::SettingsModel *settingsModel,
                                                               MetadataIO::MetadataReadingHub *readingHub):
            m_ItemsToReadSnapshot(artworksToRead),
            m_ReadingHub(readingHub),
            m_ExiftoolProcess(nullptr),
            m_SettingsModel(settingsModel),
            m_ReadSuccess(false)
        {
        }

        ExiftoolImageReadingWorker::~ExiftoolImageReadingWorker() {
            LOG_DEBUG << "Reading worker destroyed";
        }

        void ExiftoolImageReadingWorker::process() {
            auto *asyncCoordinator = m_ReadingHub->getCoordinator();
            Helpers::AsyncCoordinatorUnlocker unlocker(asyncCoordinator);

            Q_UNUSED(unlocker);

            bool success = false;
            initWorker();

            QTemporaryFile argumentsFile;

            if (argumentsFile.open()) {
                LOG_INFO << "Created arguments file" << argumentsFile.fileName();

                QStringList exiftoolArguments = createArgumentsList();
                foreach (const QString &line, exiftoolArguments) {
                    argumentsFile.write(line.toUtf8());
#ifdef Q_OS_WIN
                    argumentsFile.write("\r\n");
#else
                    argumentsFile.write("\n");
#endif
                }

                argumentsFile.flush();

                LOG_DEBUG << "Waiting for tempfile bytes written...";
#ifdef Q_OS_WIN
                HANDLE fileHandle = (HANDLE)_get_osfhandle(argumentsFile.handle());
                bool flushResult = FlushFileBuffers(fileHandle);
                LOG_DEBUG << "Windows flush result:" << flushResult;
#else
                int fsyncResult = fsync(argumentsFile.handle());
                LOG_DEBUG << "fsync result:" << fsyncResult;
#endif
                argumentsFile.close();

                QString exiftoolPath = m_SettingsModel->getExifToolPath();
                QStringList arguments;
#ifdef Q_OS_WIN
                arguments << "-charset" << "FileName=UTF8";

                /*
            QString exiftoolCommand = QString("\"\"%1\" -charset filename=UTF8 -@ \"%2\"\"").arg(exiftoolPath).arg(tempFilename);
            LOG_DEBUG << "Windows command is:" << exiftoolCommand;
            m_ExiftoolProcess->setTextModeEnabled(false);
            m_ExiftoolProcess->setProgram("cmd.exe");
            m_ExiftoolProcess->setArguments(QStringList() << "/U" << "/c");
            m_ExiftoolProcess->setNativeArguments(exiftoolCommand);
            m_ExiftoolProcess->start();
            */
#endif
                arguments << "-@" << argumentsFile.fileName();
                LOG_INFO << "Starting exiftool process:" << exiftoolPath;
                m_ExiftoolProcess->start(exiftoolPath, arguments);

                success = m_ExiftoolProcess->waitForFinished();
                LOG_DEBUG << "Exiftool process finished.";

                int exitCode = m_ExiftoolProcess->exitCode();
                QProcess::ExitStatus exitStatus = m_ExiftoolProcess->exitStatus();

                success = success &&
                        (exitCode == 0) &&
                        (exitStatus == QProcess::NormalExit);

                LOG_INFO << "Exiftool exitcode =" << exitCode << "exitstatus =" << exitStatus;
                LOG_DEBUG << "Temporary file:" << argumentsFile.fileName();

                QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
                parseExiftoolOutput(stdoutByteArray);
            }

            m_ReadSuccess = success;
            emit stopped();
        }

        void ExiftoolImageReadingWorker::cancel() {
            LOG_INFO << "Cancelling...";

            if (m_ExiftoolProcess && m_ExiftoolProcess->state() != QProcess::NotRunning) {
                m_ExiftoolProcess->kill();
                LOG_INFO << "Exiftool process killed";
            }
        }

        void ExiftoolImageReadingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
            Q_UNUSED(exitStatus);
            LOG_DEBUG << "Exiftool finished with exitcode" << exitCode;

            QByteArray stderrByteArray = m_ExiftoolProcess->readAllStandardError();
            QString stderrText = QString::fromUtf8(stderrByteArray);
            LOG_DEBUG << "STDERR [Exiftool]:" << stderrText;
        }

        void ExiftoolImageReadingWorker::initWorker() {
            m_ExiftoolProcess = new QProcess(this);
            QObject::connect(m_ExiftoolProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                             this, &ExiftoolImageReadingWorker::innerProcessFinished);
        }

        QStringList ExiftoolImageReadingWorker::createArgumentsList() {
            QStringList arguments;
            arguments.reserve(m_ItemsToReadSnapshot.size() + 10);

            /*
         * Related to the hack in windows for UTF8-encoded paths
#ifdef Q_OS_WIN
        arguments << "-charset" << "FileName=UTF8";
#endif
*/
            arguments << "-json" << "-ignoreMinorErrors" << "-e";
            arguments << "-ObjectName" << "-Title";
            arguments << "-ImageDescription" << "-Description" << "-Caption-Abstract";
            arguments << "-Keywords" << "-Subject";
            arguments << "-DateTimeOriginal" << "-TimeZoneOffset";
            arguments << "-ImageWidth" << "-ImageHeight";
            size_t size = m_ItemsToReadSnapshot.size();
            for (size_t i = 0; i < size; ++i) {
                Models::ArtworkMetadata *metadata = m_ItemsToReadSnapshot.get(i);
                arguments << metadata->getFilepath();
            }

            return arguments;
        }

        void ExiftoolImageReadingWorker::parseExiftoolOutput(const QByteArray &output) {
            LOG_DEBUG << "Parsing JSON output of exiftool...";
            QJsonDocument document = QJsonDocument::fromJson(output);
            if (document.isArray()) {
                QJsonArray filesArray = document.array();

                int size = filesArray.size();
                for (int i = 0; i < size; ++i) {
                    const QJsonValue &fileJson = filesArray.at(i);
                    if (fileJson.isObject()) {
                        QJsonObject fileObject = fileJson.toObject();
                        std::shared_ptr<MetadataIO::OriginalMetadata> result(new MetadataIO::OriginalMetadata());
                        jsonObjectToImportResult(fileObject, result.get());

                        Q_ASSERT(!result->m_FilePath.isEmpty());

                        QImageReader reader(result->m_FilePath);
                        result->m_ImageSize = reader.size();

                        QFileInfo fi(result->m_FilePath);
                        result->m_FileSize = fi.size();

                        m_ReadingHub->push(result);

                        LOG_DEBUG << "Parsed file:" << result->m_FilePath;
                    }
                }
            } else {
                LOG_WARNING << "Exiftool Output Parsing Error";
            }
        }
    }
}
