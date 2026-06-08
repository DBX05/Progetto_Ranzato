#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <QString>
#include <QSqlDatabase>

class DBConnector {
public:
    // Restituisce true se la connessione ha successo. In caso di fallimento
    // riempie outError con il messaggio d'errore. outDb conterrà la connessione aperta.
    static bool connect(const QString &host, int port, const QString &user, const QString &password, const QString &dbName, QSqlDatabase &outDb, QString &outError);
};

#endif // DB_CONNECTOR_H
