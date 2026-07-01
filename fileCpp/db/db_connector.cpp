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
    // If dbName is relative, place the database file inside a "sql" folder.
    if (!QFileInfo(filePath).isAbsolute()) {
        QDir probeDir(QDir::currentPath());
        QString sqlDirPath;

        while (true) {
            if (probeDir.exists("sql")) {
                sqlDirPath = probeDir.filePath("sql");
                break;
            }
            if (!probeDir.cdUp()) {
                break;
            }
        }

        if (sqlDirPath.isEmpty()) {
            // Fallback: create ./sql if no existing sql directory is found.
            QDir fallbackDir(QDir::currentPath());
            if (!fallbackDir.mkpath("sql")) {
                outError = "Impossibile creare la cartella 'sql' per il database.";
                qCritical() << outError;
                return false;
            }
            sqlDirPath = fallbackDir.filePath("sql");
        }

        filePath = QDir(sqlDirPath).filePath(dbName);
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
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id     INTEGER NOT NULL,
                name        TEXT    NOT NULL,
                start_datetime TEXT NOT NULL,
                end_datetime   TEXT NOT NULL,
                description TEXT,
                priority    INTEGER DEFAULT 1,
                event_type  TEXT    DEFAULT 'Generico',
                FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
            );
        )")) {
            outError = "Failed to create 'events' table: " + q.lastError().text();
            qCritical() << outError;
            db.close();
            return false;
        }
        qDebug() << "[DBConnector] 'events' table created successfully.";

        // ---- Seed: utenti di esempio ----
        q.exec(R"(
            INSERT INTO users (name, email, password_hash) VALUES
            ('Riccardo Bianchi', 'riccardo@example.com', 'pass123'),
            ('Laura Verdi',      'laura@example.com',    'laura2024'),
            ('Marco Rossi',      'marco@example.com',    'marco_pw');
        )");
        qDebug() << "[DBConnector] Sample users inserted.";

        // ---- Seed: eventi di esempio (user_id 1 = Riccardo) ----
        q.exec(R"(
            INSERT INTO events (user_id, name, start_datetime, end_datetime, description, priority, event_type) VALUES
            (1, 'Riunione di team',    '2026-06-26 09:00:00', '2026-06-26 10:30:00', 'Review sprint settimanale',   2, 'Riunione'),
            (1, 'Allenamento',         '2026-06-26 18:00:00', '2026-06-26 19:30:00', 'Palestra — gambe',            1, 'Generico'),
            (1, 'Compleanno Marco',    '2026-06-27 20:00:00', '2026-06-27 23:00:00', 'Cena a sorpresa',             1, 'Compleanno'),
            (1, 'Visita medica',       '2026-06-28 15:00:00', '2026-06-28 16:00:00', 'Controllo annuale',           3, 'Generico'),
            (1, 'Studio PaO',          '2026-06-29 14:00:00', '2026-06-29 17:00:00', 'Preparazione esame giugno',   2, 'Generico'),
            (1, 'Call con cliente',    '2026-06-30 11:00:00', '2026-06-30 11:45:00', 'Demo progetto Qt',            2, 'Riunione'),
            (1, 'Festa laurea Laura',  '2026-07-05 19:00:00', '2026-07-05 23:59:00', '',                            1, 'Festività'),
            (2, 'Corso yoga',          '2026-06-26 07:30:00', '2026-06-26 08:30:00', '',                            1, 'Generico'),
            (2, 'Riunione dipartimento','2026-06-28 10:00:00','2026-06-28 12:00:00', 'Budget Q3',                   3, 'Riunione'),
            (3, 'Kick-off progetto',   '2026-06-27 09:00:00', '2026-06-27 11:00:00', 'Nuovo cliente enterprise',    2, 'Riunione');
        )");
        qDebug() << "[DBConnector] Sample events inserted.";

        qDebug() << "[DBConnector] Database file already exists. Running schema migration...";

        // SQLite >= 3.37 supports ADD COLUMN IF NOT EXISTS; for older versions we ignore the error.
        auto tryAlter = [&](const char* sql) {
            QSqlQuery aq(db);
            if (!aq.exec(sql))
                qWarning() << "[DBConnector] Migration note:" << aq.lastError().text();
        };

        tryAlter("ALTER TABLE events ADD COLUMN description TEXT");
        tryAlter("ALTER TABLE events ADD COLUMN priority    INTEGER DEFAULT 1");
        tryAlter("ALTER TABLE events ADD COLUMN event_type  TEXT    DEFAULT 'Generico'");

        qDebug() << "[DBConnector] Schema migration complete.";
    }

    // ---- remove old branch close ----
    // (the old else block and its closing brace are replaced above)

    // Assign the connected database object to the output parameter.
    outDb = db;
    qDebug() << "[DBConnector] Database connection established successfully to:" << filePath;
    return true;
}
