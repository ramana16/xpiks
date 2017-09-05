/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadatawritingworker.h"
#include <Common/defines.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <Models/artworkmetadata.h>
#include <Models/settingsmodel.h>
#include <Common/defines.h>
#include <MetadataIO/artworkssnapshot.h>
#include <Helpers/asynccoordinator.h>

#ifdef Q_OS_WIN
#define _X86_
#include <fileapi.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define SOURCEFILE QLatin1String("SourceFile")
#define XMP_TITLE QLatin1String("XMP:Title")
#define IPTC_OBJECTNAME QLatin1String("IPTC:ObjectName")
#define XMP_DESCRIPTION QLatin1String("XMP:Description")
#define EXIF_IMAGEDESCRIPTION QLatin1String("EXIF:ImageDescription")
#define IPTC_CAPTIONABSTRACT QLatin1String("IPTC:Caption-Abstract")
#define IPTC_KEYWORDS QLatin1String("IPTC:Keywords")
#define XMP_SUBJECT QLatin1String("XMP:Subject")

namespace libxpks {
    namespace io {
        void keywordsToJsonArray(const QStringList &keywords, QJsonArray &array) {
            int size = keywords.size();
            for (int i = 0; i < size; ++i) {
                array.append(QJsonValue(keywords.at(i)));
            }
        }

        void metadataToJsonObject(Models::ArtworkMetadata *metadata, QJsonObject &jsonObject) {
            QString title = metadata->getTitle().simplified();
            QString description = metadata->getDescription().simplified();

            if (title.isEmpty()) {
                title = description;
            }

            jsonObject.insert(SOURCEFILE, QJsonValue(metadata->getFilepath()));

            QJsonValue titleValue(title);
            jsonObject.insert(XMP_TITLE, titleValue);
            jsonObject.insert(IPTC_OBJECTNAME, titleValue);

            QJsonValue descriptionValue(description);
            jsonObject.insert(XMP_DESCRIPTION, descriptionValue);
            jsonObject.insert(EXIF_IMAGEDESCRIPTION, descriptionValue);
            jsonObject.insert(IPTC_CAPTIONABSTRACT, descriptionValue);

            QStringList keywords = metadata->getKeywords();
            QJsonArray keywordsArray;
            keywordsToJsonArray(keywords, keywordsArray);
            jsonObject.insert(IPTC_KEYWORDS, keywordsArray);
            jsonObject.insert(XMP_SUBJECT, keywordsArray);
        }

        void artworksToJsonArray(const MetadataIO::ArtworksSnapshot &itemsToWrite, QJsonArray &array) {
            size_t size = itemsToWrite.size();
            for (size_t i = 0; i < size; ++i) {
                QJsonObject artworkObject;
                metadataToJsonObject(itemsToWrite.get(i), artworkObject);
                array.append(artworkObject);
            }
        }

        ExiftoolImageWritingWorker::ExiftoolImageWritingWorker(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                                               Helpers::AsyncCoordinator *asyncCoordinator,
                                                               Models::SettingsModel *settingsModel,
                                                               bool useBackups):
            m_ExiftoolProcess(nullptr),
            m_ItemsToWriteSnapshot(artworksToWrite),
            m_AsyncCoordinator(asyncCoordinator),
            m_SettingsModel(settingsModel),
            m_UseBackups(useBackups),
            m_WriteSuccess(false)
        {
            Q_ASSERT(asyncCoordinator != nullptr);
            Q_ASSERT(settingsModel != nullptr);
            Q_ASSERT(!m_ItemsToWriteSnapshot.empty());
        }

        ExiftoolImageWritingWorker::~ExiftoolImageWritingWorker() {
            LOG_DEBUG << "destroyed";
        }

        void ExiftoolImageWritingWorker::process() {
            Helpers::AsyncCoordinatorUnlocker unlocker(m_AsyncCoordinator);
            Q_UNUSED(unlocker);

            bool success = false;

            initWorker();

            QTemporaryFile jsonFile;
            if (jsonFile.open()) {
                LOG_INFO << "Serializing artworks to json" << jsonFile.fileName();
                QJsonArray objectsToSave;
                artworksToJsonArray(m_ItemsToWriteSnapshot, objectsToSave);
                QJsonDocument document(objectsToSave);
                jsonFile.write(document.toJson());
                jsonFile.flush();
                jsonFile.close();

                int numberOfItems = (int)m_ItemsToWriteSnapshot.size();

                QTemporaryFile argumentsFile;
                if (argumentsFile.open()) {
                    QStringList exiftoolArguments = createArgumentsList(jsonFile.fileName());

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
#endif
                    arguments << "-IPTC:CodedCharacterSet=UTF8" << "-@" << argumentsFile.fileName();

                    LOG_DEBUG << "Starting exiftool process:" << exiftoolPath;

                    m_ExiftoolProcess->start(exiftoolPath, arguments);
                    const int oneFileTimeout = 5000;
                    success = m_ExiftoolProcess->waitForFinished(oneFileTimeout*numberOfItems);

                    LOG_DEBUG << "Exiftool process finished.";

                    int exitCode = m_ExiftoolProcess->exitCode();
                    QProcess::ExitStatus exitStatus = m_ExiftoolProcess->exitStatus();

                    success = success &&
                            (exitCode == 0) &&
                            (exitStatus == QProcess::NormalExit);

                    LOG_INFO << "Exiftool exitcode =" << exitCode << "exitstatus =" << exitStatus;
                    LOG_DEBUG << "Temporary file:" << argumentsFile.fileName();

                    if (!success) {
                        LOG_WARNING << "Exiftool error string:" << m_ExiftoolProcess->errorString();
                    } else {
                        setArtworksSaved();
                    }
                }
            }

            m_WriteSuccess = success;
            emit stopped();
        }

        void ExiftoolImageWritingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
            Q_UNUSED(exitStatus);
            LOG_DEBUG << "Exiftool finished with exitcode" << exitCode;

            QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
            QString stdoutText = QString::fromUtf8(stdoutByteArray);
            LOG_DEBUG << "STDOUT [ExifTool]:" << stdoutText;

            QByteArray stderrByteArray = m_ExiftoolProcess->readAllStandardError();
            QString stderrText = QString::fromUtf8(stderrByteArray);
            LOG_DEBUG << "STDERR [Exiftool]:" << stderrText;
        }

        void ExiftoolImageWritingWorker::initWorker() {
            m_ExiftoolProcess = new QProcess(this);
            QObject::connect(m_ExiftoolProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                             this, &ExiftoolImageWritingWorker::innerProcessFinished);
        }

        QStringList ExiftoolImageWritingWorker::createArgumentsList(const QString &jsonFilePath) {
            QStringList arguments;
            arguments.reserve(m_ItemsToWriteSnapshot.size() + 5);

            //#ifdef Q_OS_WIN
            //        arguments << "-charset" << "FileName=UTF8";
            //#endif
            // ignore minor warnings
            arguments << "-m" << "-j=" + jsonFilePath;

            if (!m_UseBackups) {
                arguments << "-overwrite_original";
            }

            size_t size = m_ItemsToWriteSnapshot.size();
            for (size_t i = 0; i < size; ++i) {
                Models::ArtworkMetadata *metadata = m_ItemsToWriteSnapshot.get(i);
                arguments << metadata->getFilepath();
            }

            return arguments;
        }

        void ExiftoolImageWritingWorker::setArtworksSaved() {
            auto &items = m_ItemsToWriteSnapshot.getRawData();
            for (auto &item: items) {
                Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
                artwork->resetModified();
            }
        }
    }
}
