// db_connector.cpp
#include "db_connector.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

/**
 * @brief Connects to a SQLite database. Creates the database file and necessary tables if they don't exist.
 *
 * @param dbName The name of the database file (e.g., "agendadb.db"). Can be relative or absolute path.
 * @param outDb The QSqlDatabase object to use after successful connection.
 * @param outError A string reference to store any error messages.
 * @return true if the connection was successful, false otherwise.
 */
bool DBConnector::connect(const QString &dbName, QSqlDatabase &outDb, QString &outError)
{
    QString filePath = dbName;
    // If the provided dbName is not an absolute path, prepend the current working directory.
    if (!QFileInfo(filePath).isAbsolute()) {
        filePath = QDir::currentPath() + "/" + dbName;
    }

    qDebug() << "[DBConnector] Attempting to connect to database file:" << filePath;

    // Check if the database file already exists. This is used to determine if table creation is needed.
    bool dbFileExisted = QFile::exists(filePath);

    // Add a new SQLite database connection. QSQLITE is the driver name.
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid()) {
        outError = "SQLite driver is not available. Please ensure it's installed and accessible.";
        qCritical() << outError;
        return false;
    }

    db.setDatabaseName(filePath);

    // Attempt to open the database connection.
    if (!db.open()) {
        outError = "Failed to open database connection: " + db.lastError().text();
        qCritical() << outError;
        return false;
    }

    // If the database file did not exist before opening, create the tables.
    if (!dbFileExisted) {
        qDebug() << "[DBConnector] Database file did not exist. Creating tables...";
        QSqlQuery q(db); // Use a query object to execute SQL commands

        // Create users table
        if (!q.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT NOT NULL UNIQUE,
                password_hash TEXT NOT NULL
            );
        )")) {
            outError = "Failed to create 'users' table: " + q.lastError().text();
            qCritical() << outError;
            db.close(); // Close connection on critical error
            return false;
        }
        qDebug() << "[DBConnector] 'users' table created successfully.";

        // Create events table
        if (!q.exec(R"(
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                name TEXT NOT NULL,
                start_datetime TEXT NOT NULL,
                end_datetime TEXT NOT NULL,
                description TEXT,        -- Added description field
                priority INTEGER DEFAULT 1, -- Added priority field (default to 1)
                event_type TEXT,         -- Added event_type field
                FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
            );
        )")) {
            outError = "Failed to create 'events' table: " + q.lastError().text();
            qCritical() << outError;
            db.close(); // Close connection on critical error
            return false;
        }
        qDebug() << "[DBConnector] 'events' table created successfully.";

         // Create event_types table (optional, for enum-like types)
        if (!q.exec(R"(
            CREATE TABLE IF NOT EXISTS event_types (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                type_name TEXT NOT NULL UNIQUE
            );
        )")) {
            outError = "Failed to create 'event_types' table: " + q.lastError().text();
            qCritical() << outError;
            db.close();
            return false;
        }
        qDebug() << "[DBConnector] 'event_types' table created successfully.";
        // Optionally insert default types here if needed
    } else {
        qDebug() << "[DBConnector] Database file already exists. Skipping table creation.";
    }

    // Assign the connected database object to the output parameter.
    outDb = db;
    qDebug() << "[DBConnector] Database connection established successfully to:" << filePath;
    return true;
}
