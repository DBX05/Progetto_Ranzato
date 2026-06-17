#include "UserAuthDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>

UserAuthDialog::UserAuthDialog(QSqlDatabase db, QWidget* parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle("Login / Registrazione Utente");
    auto* layout = new QVBoxLayout(this);

    auto* nameLayout = new QHBoxLayout;
    nameLayout->addWidget(new QLabel("Nome:"));
    m_name = new QLineEdit(this);
    nameLayout->addWidget(m_name);
    layout->addLayout(nameLayout);

    auto* emailLayout = new QHBoxLayout;
    emailLayout->addWidget(new QLabel("Email:"));
    m_email = new QLineEdit(this);
    emailLayout->addWidget(m_email);
    layout->addLayout(emailLayout);

    auto* passLayout = new QHBoxLayout;
    passLayout->addWidget(new QLabel("Password:"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    passLayout->addWidget(m_password);
    layout->addLayout(passLayout);

    auto* btnLayout = new QHBoxLayout;
    m_ok = new QPushButton("Login / Register", this);
    m_cancel = new QPushButton("Annulla", this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_ok);
    btnLayout->addWidget(m_cancel);
    layout->addLayout(btnLayout);

    connect(m_ok, &QPushButton::clicked, this, [this]() {
        // Prima prova login, se fallisce prova registrazione
        if (tryLogin()) {
            accept();
        } else {
            // chiedi conferma registrazione
            auto ret = QMessageBox::question(this, "Utente non trovato", "Utente non trovato. Vuoi registrarlo?");
            if (ret == QMessageBox::Yes) {
                if (tryRegister()) accept();
            }
        }
    });
    connect(m_cancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString UserAuthDialog::hashPassword(const QString& plain) const {
    QByteArray ba = plain.toUtf8();
    QByteArray hash = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool UserAuthDialog::tryLogin() {
    if (!m_db.isValid() || !m_db.isOpen()) return false;
    QString email = m_email->text().trimmed();
    QString passHash = hashPassword(m_password->text());

    QSqlQuery q(m_db);
    q.prepare("SELECT id, name, email FROM users WHERE email = ? AND password_hash = ?");
    q.addBindValue(email);
    q.addBindValue(passHash);
    if (!q.exec()) {
        qDebug() << "Login query error:" << q.lastError().text();
        return false;
    }
    if (q.next()) {
        m_result.userId = q.value(0).toInt();
        m_result.name = q.value(1).toString();
        m_result.email = q.value(2).toString();
        return true;
    }
    return false;
}

bool UserAuthDialog::tryRegister() {
    if (!m_db.isValid() || !m_db.isOpen()) return false;
    QString name = m_name->text().trimmed();
    QString email = m_email->text().trimmed();
    QString passHash = hashPassword(m_password->text());

    if (name.isEmpty() || email.isEmpty() || m_password->text().isEmpty()) {
        QMessageBox::warning(this, "Dati mancanti", "Inserisci nome, email e password per registrarti.");
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (name, email, password_hash) VALUES (?, ?, ?)");
    q.addBindValue(name);
    q.addBindValue(email);
    q.addBindValue(passHash);
    if (!q.exec()) {
        qDebug() << "Register error:" << q.lastError().text();
        QMessageBox::warning(this, "Errore registrazione", q.lastError().text());
        return false;
    }
    // recupera id
    QVariant lastId = q.lastInsertId();
    m_result.userId = lastId.isValid() ? lastId.toInt() : -1;
    m_result.name = name;
    m_result.email = email;
    return true;
}

AuthResult UserAuthDialog::result() const {
    return m_result;
}
