#include <QApplication>
#include "MainWindow.h"
#include "../auth/DBConnectDialog.h"
#include "../auth/UserAuthDialog.h"
#include "../db/db_connector.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1) chiedi parametri DB
    DBConnectDialog dbDlg;
    if (dbDlg.exec() != QDialog::Accepted) return 0;
    DBConnectParams params = dbDlg.params();

    // 2) connetti DB
    QSqlDatabase db;
    QString err;
    if (!DBConnector::connect(params.host, params.port, params.user, params.password, params.dbName, db, err)) {
        QMessageBox::critical(nullptr, "DB Error", "Connessione fallita: " + err);
        return 1;
    }

    // 3) chiedi credenziali utente (login/registrazione)
    UserAuthDialog authDlg(db);
    if (authDlg.exec() != QDialog::Accepted) return 0;
    AuthResult user = authDlg.result();

    // 4) apri MainWindow e passa DB e user info
    MainWindow w(db, user.userId, user.name, user.email);
    w.show();
    return a.exec();
}
