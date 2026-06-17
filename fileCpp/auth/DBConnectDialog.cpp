// db_connector.cpp (MySQL)
#include "db_connector.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

bool DBConnector::connect(const QString &host, int port, const QString &user, const QString &password, const QString &dbName, QSqlDatabase &outDb, QString &outError)
{
    // Controlla driver disponibile
    QStringList drivers = QSqlDatabase::drivers();
    if (!drivers.contains("QMYSQL")) {
        outError = "QMYSQL driver not available. Install/enable Qt SQL MySQL plugin.";
        return false;
    }

    // Nome connessione unico
    const QString connName = "connector_connection_mysql";
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase::removeDatabase(connName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connName);
    db.setHostName(host.isEmpty() ? "127.0.0.1" : host);
    if (port > 0) db.setPort(port);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        outError = db.lastError().text();
        qDebug() << "DB open error:" << outError;
        return false;
    }

    // Crea tabelle se non esistono (MySQL syntax)
    QSqlQuery q(db);
    const QString createUsers = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "name VARCHAR(255) NOT NULL,"
        "email VARCHAR(255) NOT NULL UNIQUE,"
        "password_hash VARCHAR(255) NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ") ENGINE=InnoDB;"
    );
    if (!q.exec(createUsers)) {
        outError = q.lastError().text();
        db.close();
        return false;
    }

    const QString createEvents = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS events ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "user_id INT NOT NULL,"
        "name VARCHAR(255) NOT NULL,"
        "start_datetime DATETIME NOT NULL,"
        "end_datetime DATETIME NOT NULL,"
        "type INT NOT NULL DEFAULT 0,"
        "description TEXT,"
        "priority INT DEFAULT 1,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ") ENGINE=InnoDB;"
    );
    if (!q.exec(createEvents)) {
        outError = q.lastError().text();
        db.close();
        return false;
    }

    outDb = db;
    return true;
}
