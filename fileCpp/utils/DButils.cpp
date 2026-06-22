/*#include "DButils.h"
#include <QSqlError>
#include <QDebug>

QSqlDatabase openAgendaDb()
{
    const QString connName = "AgendaConnection";

    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if (db.isOpen()) return db;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connName);

    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("agendadb");
    db.setUserName("root");
    db.setPassword("");

    if (!db.open()) {
        qDebug() << "Errore apertura DB:" << db.lastError().text();
        return QSqlDatabase();
    }

    qDebug() << "Connessione MySQL OK";
    return db;
}
*/