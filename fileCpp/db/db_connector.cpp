// db_connector.cpp
#include "db_connector.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

bool DBConnector::connect(const QString &dbName, QSqlDatabase &outDb, QString &outError)
{
    QString filePath = dbName;
    if (!QFileInfo(filePath).isAbsolute())
        filePath = QDir::currentPath() + "/" + filePath;

    qDebug() << "[DBConnector] Percorso DB:" << filePath;

    bool existed = QFile::exists(filePath);

    // connessione predefinita
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid()) {
        outError = "Driver SQLite non disponibile";
        return false;
    }

    db.setDatabaseName(filePath);
    if (!db.open()) {
        outError = db.lastError().text();
        return false;
    }

    if (!existed) {
        QSqlQuery q(db);
        q.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT NOT NULL UNIQUE,
                password_hash TEXT NOT NULL
            );
        )");
        q.exec(R"(
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                name TEXT NOT NULL,
                start_datetime TEXT NOT NULL,
                end_datetime TEXT NOT NULL,
                FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
            );
        )");
    }

    outDb = db;
    return true;
}
