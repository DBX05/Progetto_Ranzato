#include "db_connector.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>

bool DBConnector::connect(const QString &host, int /*port*/, const QString &user, const QString &password, const QString &dbName, QSqlDatabase &outDb, QString &outError)
{
    Q_UNUSED(host);
    Q_UNUSED(user);
    Q_UNUSED(password);

    QString filePath = dbName;
    if (filePath.isEmpty()) filePath = "agendadb.db";

    // Usa connessione con nome unico per evitare conflitti
    const QString connName = "connector_connection";
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase::removeDatabase(connName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(filePath);

    if (!db.open()) {
        outError = db.lastError().text();
        return false;
    }

   // Crea tabelle se non esistono: users e events
QSqlQuery q(db);

q.exec(R"(
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);
)");

q.exec(R"(
CREATE TABLE IF NOT EXISTS events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    name TEXT NOT NULL,
    start_datetime TEXT NOT NULL,
    end_datetime TEXT NOT NULL,
    type INTEGER NOT NULL DEFAULT 0,
    description TEXT,
    priority INTEGER DEFAULT 1,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);
)");

    outDb = db;
    return true;
}

int ensureUserExists(QSqlDatabase& db,
                     const QString& name,
                     const QString& email,
                     const QString& password)
{
    if (!db.isOpen())
        return -1;

    // 1. Controlla se l'utente esiste già
    {
        QSqlQuery q(db);
        q.prepare("SELECT id FROM users WHERE email = :email");
        q.bindValue(":email", email);

        if (q.exec() && q.next()) {
            return q.value(0).toInt(); // utente già esistente
        }
    }

    // 2. Inserisce nuovo utente
    {
        QSqlQuery q(db);
        q.prepare(R"(
            INSERT INTO users (name, email, password_hash)
            VALUES (:name, :email, :password_hash)
        )");

        q.bindValue(":name", name);
        q.bindValue(":email", email);
        q.bindValue(":password_hash", password);

        if (!q.exec()) {
            qWarning() << "Errore creazione utente:" << q.lastError().text();
            return -1;
        }

        return q.lastInsertId().toInt();
    }
}
