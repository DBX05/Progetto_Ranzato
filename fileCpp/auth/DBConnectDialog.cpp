// DBConnectDialog.cpp
#include "DBConnectDialog.h"
#include "../db/db_connector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSqlDatabase>
#include <QMessageBox>


DBConnectDialog::DBConnectDialog(QWidget* parent)
    : QDialog(parent)
{



    setWindowTitle("Connessione al Database");
    auto* layout = new QVBoxLayout(this);

    auto* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(new QLabel("Database name / SQLite file:"));
    m_dbName = new QLineEdit(this);
    m_dbName->setText("agendadb.db"); 
    dbLayout->addWidget(m_dbName);
    layout->addLayout(dbLayout);

    // Label per messaggi di errore / validazione
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: #b00020;"); // rosso
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setText("");
    layout->addWidget(m_errorLabel);

    auto* btnLayout = new QHBoxLayout;
    m_ok = new QPushButton("Connetti", this);
    m_cancel = new QPushButton("Annulla", this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_ok);
    btnLayout->addWidget(m_cancel);
    layout->addLayout(btnLayout);

    connect(m_cancel, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_ok, &QPushButton::clicked, this, [this]() {

        QString dbName = m_dbName->text().trimmed();
        if (dbName.isEmpty()) {
            QMessageBox::warning(this, "Errore", "Inserisci un nome di database valido.");
            return;
        }

        QSqlDatabase db;
        QString err;

        // CHIAMATA CORRETTA AL TUO DBConnector
        if (!DBConnector::connect(dbName, db, err)) {
            QMessageBox::warning(this, "Errore connessione", err);
            return;     // NON chiudere il dialogo
        }
        qDebug() << "CONNESSIONE OK";


        // Connessione riuscita
        accept();
    });

}

DBConnectParams DBConnectDialog::params() const {
    DBConnectParams p;
    p.host = m_host->text();
    p.port = m_port->value();
    p.user = m_user->text();
    p.password = m_password->text();
    p.dbName = m_dbName->text();
    return p;
}

void DBConnectDialog::validateInputs()
{
    QString host = m_host->text().trimmed();
    QString user = m_user->text().trimmed();
    QString dbName = m_dbName->text().trimmed();
    int port = m_port->value();

    // Regole di validazione:
    // - dbName non può essere vuoto
    // - se host è vuoto consideriamo che l'utente probabilmente vuole SQLite: in questo caso user può essere vuoto
    // - se host non è vuoto (uso MySQL), richiediamo user non vuoto e port > 0
    bool hostEmpty = host.isEmpty();
    bool dbNameEmpty = dbName.isEmpty();
    bool userEmpty = user.isEmpty();

    if (dbNameEmpty) {
        m_errorLabel->setText("Nome database o file SQLite obbligatorio.");
        m_ok->setEnabled(false);
        return;
    }

    if (!hostEmpty) {
        // validazione host: semplice controllo formato (almeno un carattere non spazio)
        if (host.length() < 3) {
            m_errorLabel->setText("Host non valido.");
            m_ok->setEnabled(false);
            return;
        }
        // per MySQL richiediamo username e porta > 0
        if (userEmpty) {
            m_errorLabel->setText("Inserisci username per la connessione MySQL.");
            m_ok->setEnabled(false);
            return;
        }
        if (port <= 0) {
            m_errorLabel->setText("Inserisci una porta valida (>0) per MySQL.");
            m_ok->setEnabled(false);
            return;
        }
    }

    // Se arriviamo qui, i campi sono considerati validi
    m_errorLabel->setText("");
    m_ok->setEnabled(true);
}
