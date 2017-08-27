/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "database.h"
#include <QDir>
#include <string>
#include <cstring>
#include <cmath>
#include <climits>
#include "../../vendors/sqlite/sqlite3.h"
#include "../Common/defines.h"
#include "../Helpers/stringhelper.h"
#include "constants.h"

#define MEGABYTE (1024*1024)
#define MAX_BLOB_BYTES (10*MEGABYTE)

namespace Helpers {
    QString ensureDBDirectoryExists(const QString &dbDirName) {
        QString appDataPath = XPIKS_USERDATA_PATH;
        QString path;

        if (!appDataPath.isEmpty()) {
            path = QDir::cleanPath(appDataPath + QDir::separator() + dbDirName);

            QDir dbDir(path);
            if (!dbDir.exists()) {
                LOG_INFO << "Creating db dir" << path;
                QDir().mkpath(path);
            }
        } else {
            path = QDir::currentPath();
        }

        return path;
    }

    bool readSqliteBlob(sqlite3_stmt *statement, int index, QByteArray &result) {
        Q_ASSERT(statement != nullptr);

        bool success = false;

        do {
            int resultColumnType = sqlite3_column_type(statement, index);
            if (resultColumnType != SQLITE_BLOB) {
                LOG_WARNING << "Table structure is incorrect";
                Q_ASSERT(false);
                break;
            }

            const void *blobData = sqlite3_column_blob(statement, index);

            int blobBytes = sqlite3_column_bytes(statement, index);
            if ((blobBytes <= 0) || (blobBytes > MAX_BLOB_BYTES)) {
                LOG_WARNING << "Stored blob has incorrect size";
                break;
            }

            result.clear();
            result.reserve(blobBytes);
            result.append((const char *)blobData, blobBytes);

            success = true;
        } while (false);

        return success;
    }

    void cleanupSqliteStatement(sqlite3_stmt *statement) {
        Q_ASSERT(statement != nullptr);
        int rc = 0;

        rc = sqlite3_clear_bindings(statement);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to clear bindings from statement. Error:" << sqlite3_errstr(rc);
        }

        rc = sqlite3_reset(statement);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to reset statement. Error:" << sqlite3_errstr(rc);
        }
    }

    bool bindSqliteBlob(sqlite3_stmt *statement, int index, const QByteArray &blob) {
        Q_ASSERT(statement != nullptr);
        bool anyFault = false;

        const char *blobData = blob.data();
        int blobLength = blob.size();

        int rc = sqlite3_bind_blob(statement, index, blobData, blobLength, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to bind statement's blob. Error:" << sqlite3_errstr(rc);
            anyFault = true;
        }

        return !anyFault;
    }

    void finalizeSqliteStatement(sqlite3_stmt *statement) {
        if (statement != nullptr) {
            int rc = sqlite3_finalize(statement);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to finalize statement. Error:" << sqlite3_errstr(rc);
            }
        }
    }

    Database::Database(int id, AsyncCoordinator *finalizeCoordinator):
        m_ID(id),
        m_FinalizeCoordinator(finalizeCoordinator),
        m_IsOpened(false)
    {
        AsyncCoordinatorLocker locker(m_FinalizeCoordinator);
        Q_UNUSED(locker);
    }

    Database::~Database() {
        Q_ASSERT(m_IsOpened == false);

        if (m_IsOpened) {
            close();
        }
    }

    Database::Transaction::Transaction(sqlite3 *database):
        m_Database(database),
        m_Started(false)
    {
        Q_ASSERT(database != nullptr);
        int rc = sqlite3_exec(m_Database, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc == SQLITE_OK) {
            m_Started = true;
        } else {
            LOG_WARNING << "Failed to begin a transaction!";
        }
    }

    Database::Transaction::~Transaction() {
        if (!m_Started) { return; }

        int rc = sqlite3_exec(m_Database, "END TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to end a transaction!";
        }
    }

    bool Database::open(const char *fullDbPath) {
        LOG_INFO << "#" << m_ID << fullDbPath;
        bool anyError = false;

        int flags = 0;
        flags |= SQLITE_OPEN_READWRITE;
        flags |= SQLITE_OPEN_CREATE;
        flags |= SQLITE_OPEN_FULLMUTEX;

        const int result = sqlite3_open_v2(fullDbPath, &m_Database, flags, nullptr);
        if (result != SQLITE_OK) {
            LOG_WARNING << "Opening" << fullDbPath << "failed! Error:" << sqlite3_errstr(result);
            anyError = true;

            doClose();
        } else {
            m_IsOpened = true;
            LOG_INFO << "Database" << fullDbPath << "has been opened";
        }

        return !anyError;
    }

    void Database::close() {
        if (m_IsOpened) {
            doClose();
        } else {
            LOG_WARNING << "Database is not opened. Skipping closing.";
        }
    }

    bool Database::initialize() {
        LOG_DEBUG << "#" << m_ID;
        Q_ASSERT(m_IsOpened);
        Q_ASSERT(m_Database != nullptr);

        executeStatement("PRAGMA auto_vacuum = 0;");
        executeStatement("PRAGMA cache_size = -20000;");
        executeStatement("PRAGMA case_sensitive_like = true;");
        executeStatement("PRAGMA encoding = \"UTF-8\";");
        executeStatement("PRAGMA journal_mode = WAL;");
        executeStatement("PRAGMA locking_mode = EXCLUSIVE;");
        executeStatement("PRAGMA synchronous = NORMAL;");
        // executeStatement("PRAGMA quick_check;");

        return true;
    }

    void Database::finalize() {
        LOG_DEBUG << "#" << m_ID;

        for (auto &table: m_Tables) {
            table->finalize();
        }
    }

    void Database::sync() {
        LOG_DEBUG << "#" << m_ID;
        int rc = sqlite3_wal_checkpoint_v2(m_Database, nullptr, SQLITE_CHECKPOINT_PASSIVE, 0, 0);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to checkpoint WAL. Error:" << sqlite3_errstr(rc);
        }
    }

    std::shared_ptr<Database::Table> Database::getTable(const QString &name) {
        LOG_DEBUG << "#" << m_ID << name;
        std::shared_ptr<Database::Table> table;

        QString createSql = QString("CREATE TABLE IF NOT EXISTS %1 ("
                                    "key BLOB PRIMARY KEY NOT NULL,"
                                    "value BLOB);").arg(name);
        std::string createStr = createSql.toStdString();

        int rc = sqlite3_exec(m_Database, createStr.c_str(), nullptr, nullptr, nullptr);
        if (rc == SQLITE_OK) {
            table.reset(new Database::Table(m_Database, name));

            if (table->initialize()) {
                m_Tables.push_back(table);
            } else {
                LOG_WARNING << "Initializing the table" << name << "failed";
                table->finalize();
                table.reset();
            }
        } else {
            LOG_WARNING << "Creating a table failed! Error:" << sqlite3_errstr(rc);
        }

        return table;
    }

    Database::Table::Table(sqlite3 *database, const QString &tableName):
        m_TableName(tableName),
        m_Database(database),
        m_GetStatement(nullptr),
        m_SetStatement(nullptr),
        m_AddStatement(nullptr),
        m_DelStatement(nullptr),
        m_AllStatement(nullptr)
    {
        Q_ASSERT(database != nullptr);
        Q_ASSERT(Helpers::is7BitAscii(tableName.toUtf8()));
    }

    bool Database::Table::initialize() {
        LOG_DEBUG << m_TableName;
        Q_ASSERT(m_Database != nullptr);

        int rc = 0;
        bool anyError = false;

        do {
            std::string selectStr = QString("SELECT value FROM %1 WHERE key=?").arg(m_TableName).toStdString();
            rc = sqlite3_prepare_v2(m_Database, selectStr.c_str(), -1, &m_GetStatement, 0);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to prepare GET statement:" << sqlite3_errstr(rc);
                anyError = true;
                break;
            }

            std::string insertStr = QString("INSERT OR REPLACE INTO %1 (key, value) VALUES (?, ?)").arg(m_TableName).toStdString();
            rc = sqlite3_prepare_v2(m_Database, insertStr.c_str(), -1, &m_SetStatement, 0);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to prepare SET statement:" << sqlite3_errstr(rc);
                anyError = true;
                break;
            }

            std::string addStr = QString("INSERT OR IGNORE INTO %1 (key, value) VALUES (?, ?)").arg(m_TableName).toStdString();
            rc = sqlite3_prepare_v2(m_Database, addStr.c_str(), -1, &m_AddStatement, 0);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to prepare ADD statement:" << sqlite3_errstr(rc);
                anyError = true;
                break;
            }

            std::string deleteStr = QString("DELETE FROM %1 WHERE key=?").arg(m_TableName).toStdString();
            rc = sqlite3_prepare_v2(m_Database, deleteStr.c_str(), -1, &m_DelStatement, 0);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to prepare DEL statement:" << sqlite3_errstr(rc);
                anyError = true;
                break;
            }

            std::string selectAllStr = QString("SELECT key, value FROM %1").arg(m_TableName).toStdString();
            rc = sqlite3_prepare_v2(m_Database, selectAllStr.c_str(), -1, &m_AllStatement, 0);
            if (rc != SQLITE_OK) {
                LOG_WARNING << "Failed to prepare SELECT ALL statement:" << sqlite3_errstr(rc);
                anyError = true;
                break;
            }
        } while (false);

        return !anyError;
    }

    void Database::Table::finalize() {
        LOG_DEBUG << m_TableName;

        finalizeSqliteStatement(m_GetStatement);
        finalizeSqliteStatement(m_SetStatement);
        finalizeSqliteStatement(m_AddStatement);
        finalizeSqliteStatement(m_DelStatement);
        finalizeSqliteStatement(m_AllStatement);
    }

    bool Database::Table::tryGetValue(const QByteArray &key, QByteArray &value) {
        Q_ASSERT(m_GetStatement != nullptr);
        Q_ASSERT(!key.isEmpty());
        if (key.isEmpty()) { return false; }

        int rc = 0;
        bool success = false;

        LOG_INTEGR_TESTS_OR_DEBUG << key;

        do {
            if (!bindSqliteBlob(m_GetStatement, 1, key)) { break; }

            rc = sqlite3_step(m_GetStatement);
            if (rc != SQLITE_ROW) {
                if (rc != SQLITE_DONE) {
                    LOG_WARNING << "Failed to step GET statement. Error:" << sqlite3_errstr(rc);
                }
                break;
            }

            if (!readSqliteBlob(m_GetStatement, 0, value)) { break; }

            rc = sqlite3_step(m_GetStatement);
            if (rc != SQLITE_DONE) {
                LOG_WARNING << "Table structure is wrong: another row with same key seems to exist.";
                Q_ASSERT(false);
                break;
            }

            success = true;
        } while (false);

        cleanupSqliteStatement(m_GetStatement);

        return success;
    }

    bool Database::Table::trySetValue(const QByteArray &key, const QByteArray &value) {
        Q_ASSERT(m_SetStatement != nullptr);
        Q_ASSERT(!key.isEmpty());
        Q_ASSERT(!value.isEmpty());

        LOG_INTEGR_TESTS_OR_DEBUG << key << "->" << "[" << value.size() << "] bytes";

        if (key.isEmpty() || value.isEmpty()) { return false; }

        bool success = false;
        int rc = 0;

        do {
            if (!bindSqliteBlob(m_SetStatement, 1, key)) { break; }
            if (!bindSqliteBlob(m_SetStatement, 2, value)) { break; }

            rc = sqlite3_step(m_SetStatement);
            if (rc != SQLITE_DONE) {
                LOG_WARNING << "Failed to step SET statement. Error:" << sqlite3_errstr(rc);
                break;
            }

            success = true;
        } while (false);

        cleanupSqliteStatement(m_SetStatement);

        return success;
    }

    bool Database::Table::tryAddValue(const QByteArray &key, const QByteArray &value) {
        Q_ASSERT(m_AddStatement != nullptr);
        Q_ASSERT(!key.isEmpty());
        Q_ASSERT(!value.isEmpty());

        LOG_INTEGR_TESTS_OR_DEBUG << key << "->" << "[" << value.size() << "] bytes";

        if (key.isEmpty() || value.isEmpty()) { return false; }

        bool success = false;
        int rc = 0;

        do {
            if (!bindSqliteBlob(m_AddStatement, 1, key)) { break; }
            if (!bindSqliteBlob(m_AddStatement, 2, value)) { break; }

            rc = sqlite3_step(m_AddStatement);
            if (rc != SQLITE_DONE) {
                LOG_WARNING << "Failed to step ADD statement. Error:" << sqlite3_errstr(rc);
                break;
            }

            success = true;
        } while (false);

        cleanupSqliteStatement(m_AddStatement);

        return success;
    }

    bool Database::Table::trySetMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList, QVector<int> &failedIndices) {
        Q_ASSERT(m_SetStatement != nullptr);
        LOG_DEBUG << keyValueList.size() << "pair(s)";

        bool anyError = false;

        Transaction t(m_Database);
        Q_UNUSED(t);
        int i = 0;

        for (auto &keyValue: keyValueList) {
            bool success = trySetValue(keyValue.first, keyValue.second);

            if (!success) {
                LOG_WARNING << "Failed to set" << keyValue.first;
                anyError = true;
                failedIndices.append(i);
            }

            i++;
        }

        return !anyError;
    }

    int Database::Table::tryAddMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList) {
        Q_ASSERT(m_AddStatement != nullptr);
        LOG_DEBUG << keyValueList.size() << "pair(s)";

        int addedCount = 0;

        Transaction t(m_Database);
        Q_UNUSED(t);

        for (auto &keyValue: keyValueList) {
            bool success = tryAddValue(keyValue.first, keyValue.second);

            if (success) {
                addedCount++;
            } else {
                LOG_INTEGRATION_TESTS << "Already exists" << keyValue.first;
            }
        }

        return addedCount;
    }

    bool Database::Table::tryDeleteRecord(const QByteArray &key) {
        Q_ASSERT(m_DelStatement != nullptr);
        Q_ASSERT(!key.isEmpty());
        if (key.isEmpty()) { return false; }

        int rc = 0;
        bool success = false;

        LOG_INTEGR_TESTS_OR_DEBUG << key;

        do {
            if (!bindSqliteBlob(m_DelStatement, 1, key)) { break; }

            rc = sqlite3_step(m_DelStatement);
            if (rc != SQLITE_DONE) {
                LOG_WARNING << "Failed to step DEL statement. Error:" << sqlite3_errstr(rc);
                break;
            }

            success = true;
        } while (false);

        cleanupSqliteStatement(m_DelStatement);

        return success;
    }

    bool Database::Table::tryDeleteMany(const QVector<QByteArray> &keysList) {
        Q_ASSERT(m_DelStatement != nullptr);
        LOG_DEBUG << keysList.size() << "key(s)";

        bool anyError = false;

        Transaction t(m_Database);
        Q_UNUSED(t);

        for (auto &key: keysList) {
            bool success = tryDeleteRecord(key);
            if (!success) {
                LOG_WARNING << "Failed to delete" << key;
                anyError = true;
            }
        }

        return !anyError;
    }

    void Database::Table::foreachRow(std::function<bool (QByteArray &, QByteArray &)> action) {
        Q_ASSERT(m_AllStatement != nullptr);
        LOG_DEBUG << "#";

        int rc = 0;

        do {
            while (SQLITE_ROW == (rc = sqlite3_step(m_AllStatement))) {
                QByteArray key, value;

                if (!readSqliteBlob(m_AllStatement, 0, key)) { continue; }
                if (!readSqliteBlob(m_AllStatement, 1, value)) { continue; }

                const bool shouldContinue = action(key, value);
                if (!shouldContinue) { break; }
            }

            if ((rc != SQLITE_DONE) && (rc != SQLITE_ROW)) {
                LOG_WARNING << "Error while going through the ALL statement." << sqlite3_errstr(rc);
                Q_ASSERT(false);
            }
        } while (false);

        cleanupSqliteStatement(m_AllStatement);
    }

    void Database::doClose() {
        LOG_DEBUG << "#" << m_ID;

        finalize();

        const int closeResult = sqlite3_close(m_Database);
        if (closeResult != SQLITE_OK) {
            LOG_WARNING << "Failed to close a database #" << m_ID << ". Error:" << sqlite3_errstr(closeResult);
        } else {
            LOG_DEBUG << "DB" << "#" << m_ID << "closed";
            m_IsOpened = false;
        }
    }

    bool Database::executeStatement(const char *stmt) {
        LOG_DEBUG << "#" << m_ID << stmt;

        int rc = sqlite3_exec(m_Database, stmt, nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            LOG_WARNING << "Failed to execute (" << stmt << "). Error:" << sqlite3_errstr(rc);
        }

        bool success = rc == SQLITE_OK;
        return success;
    }

    DatabaseManager::DatabaseManager():
        QObject(),
        m_LastDatabaseID(0),
        m_Initialized(false)
    {
        QObject::connect(&m_FinalizeCoordinator, &Helpers::AsyncCoordinator::statusReported,
                         this, &DatabaseManager::onReadyToFinalize);
    }

    bool DatabaseManager::initialize() {
        LOG_DEBUG << "#";
        QString dbDirPath = ensureDBDirectoryExists(Constants::DB_DIR);
        m_DBDirPath = dbDirPath;
        m_Initialized = true;
        return true;
    }

    void DatabaseManager::finalize() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_Initialized);
        closeAll();
        m_Initialized = false;
    }

    std::shared_ptr<Database> DatabaseManager::openDatabase(const QString &dbName) {
        Q_ASSERT(m_Initialized);
        LOG_DEBUG << dbName;

        const int id = getNextID();
        std::shared_ptr<Database> db(new Database(id, &m_FinalizeCoordinator));

        QDir databasesDir(m_DBDirPath);
        Q_ASSERT(databasesDir.exists());
        QString fullDbPath = databasesDir.filePath(dbName);
        QByteArray utf8Path = fullDbPath.toUtf8();
        const char *dbPath = utf8Path.data();

        bool openSucceded = db->open(dbPath);
        if (!openSucceded) {
            LOG_WARNING << "Failed to open" << dbName;
            db.reset();
        } else {
            LOG_INFO << "Opened" << dbName << "database";

            QMutexLocker locker(&m_Mutex);
            Q_UNUSED(locker);
            m_DatabaseArray.push_back(db);
        }

        return db;
    }

    void DatabaseManager::prepareToFinalize() {
        LOG_DEBUG << "#";
        m_FinalizeCoordinator.allBegun();
    }

    void DatabaseManager::onReadyToFinalize(int status) {
        LOG_INFO << status;
        finalize();
    }

    void DatabaseManager::closeAll() {
        LOG_DEBUG << "#";

        for (auto &db: m_DatabaseArray) {
            db->finalize();
            db->close();
        }

        LOG_INFO << "Databases closed";
    }

    int DatabaseManager::getNextID() {
        int id = m_LastDatabaseID.fetchAndAddOrdered(1);
        return id;
    }
}
