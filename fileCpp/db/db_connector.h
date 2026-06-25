// db_connector.h
#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <QString>
#include <QSqlDatabase>

class DBConnector {
public:
    static bool connect(const QString &dbName,
                        QSqlDatabase &outDb,
                        QString &outError);
};

#endif
