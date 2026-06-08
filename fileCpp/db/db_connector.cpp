#include "db_connector.h"
#include <QSqlDatabase>
#include <QSqlError>

bool DBConnector::connect(const QString &host, int port, const QString &user, const QString &password, const QString &dbName, QSqlDatabase &outDb, QString &outError)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "connector_connection");
    db.setHostName(host);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);
    if (!db.open()) {
        outError = db.lastError().text();
        return false;
    }
    outDb = db;
    return true;
}
