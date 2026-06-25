#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "../auth/DBConnectDialog.h"
#include "../auth/UserAuthDialog.h"
#include "../db/db_connector.h"
#include "MainWindow.h"

int ensureUserExists(QSqlDatabase &db,
                     const QString &name,
                     const QString &email,
                     const QString &password)
{
    if (!db.isOpen()) {
        qDebug() << "[ensureUserExists] DB non aperto";
        return -1;
    }

    QSqlQuery q(db);

    q.prepare("SELECT id FROM users WHERE email = ?");
    q.addBindValue(email);
    if (!q.exec()) {
        qDebug() << "[ensureUserExists] select error:" << q.lastError().text();
        return -1;
    }
    if (q.next())
        return q.value(0).toInt();

    q.prepare("INSERT INTO users(name, email, password_hash) VALUES(?, ?, ?)");
    q.addBindValue(name);
    q.addBindValue(email);
    q.addBindValue(password); // per ora in chiaro
    if (!q.exec()) {
        qDebug() << "[ensureUserExists] insert error:" << q.lastError().text();
        return -1;
    }
    return q.lastInsertId().toInt();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db;
    QString err;

    if (!DBConnector::connect("agendadb.db", db, err)) {
        QMessageBox::critical(nullptr, "Errore DB", err);
        return 1;
    }
    qDebug() << "[MAIN] DB connesso, isOpen=" << db.isOpen();

    UserAuthDialog authDlg(db);
    if (authDlg.exec() != QDialog::Accepted)
        return 0;

    AuthResult ar = authDlg.result();
    int userId = ensureUserExists(db, ar.name, ar.email, ar.password);
    if (userId < 0) {
        QMessageBox::critical(nullptr, "Errore", "Impossibile creare/verificare l'utente.");
        return 1;
    }

    MainWindow w(db, userId, ar.name, ar.email);
    w.show();

    return a.exec();
}
