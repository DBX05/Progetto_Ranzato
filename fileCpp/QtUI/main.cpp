#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "../auth/DBConnectDialog.h"
#include "../auth/UserAuthDialog.h"
#include "../db/db_connector.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Rimuove eventuali connessioni precedenti
    if (QSqlDatabase::contains("connector_connection"))
        QSqlDatabase::removeDatabase("connector_connection");

    QSqlDatabase db;
    QString err;

    // --- CICLO DI CONNESSIONE ---
    while (true) {
        DBConnectDialog dlg;
        if (dlg.exec() != QDialog::Accepted) {
            return 0; // utente ha annullato
        }

        DBConnectParams params = dlg.params();

        if (!DBConnector::connect(params.host,
                                  params.port,
                                  params.user,
                                  params.password,
                                  params.dbName,
                                  db,
                                  err))
        {
            QMessageBox::warning(nullptr,
                                 "Errore connessione",
                                 "Connessione fallita. Riprova.");
            continue;
        }

        break; // connessione riuscita
    }

    // --- LOGIN / REGISTRAZIONE UTENTE ---
    UserAuthDialog authDlg(db);
    if (authDlg.exec() != QDialog::Accepted) {
        return 0;
    }

    AuthResult ar = authDlg.result();

    int userId = ensureUserExists(db, ar.name, ar.email, ar.password);
    if (userId < 0) {
        QMessageBox::warning(nullptr, "Errore", "Impossibile verificare o creare l'utente.");
        return 0;
    }

    MainWindow w(db, userId, ar.name, ar.email);
    w.show();

    return a.exec();
}
