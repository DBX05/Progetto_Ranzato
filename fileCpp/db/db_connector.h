#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <QString>
#include <QSqlDatabase>

class DBConnector {
public:
    // Restituisce true se la connessione ha successo. outDb contiene la connessione aperta.
    // host, port, user, password, dbName sono richiesti dall'interfaccia ma per sviluppo
    // il driver usato è QSQLITE (dbName = percorso file .db). Se dbName vuoto usa "agenda_local.db".
    static bool connect(const QString &host, int port, const QString &user, const QString &password, const QString &dbName, QSqlDatabase &outDb, QString &outError);
};

int ensureUserExists(QSqlDatabase& db,
                     const QString& name,
                     const QString& email,
                     const QString& password);

#endif // DB_CONNECTOR_H
