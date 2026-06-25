// UserAuthDialog.cpp
#include "UserAuthDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

UserAuthDialog::UserAuthDialog(const QSqlDatabase &db, QWidget *parent)
    : QDialog(parent),
      m_db(db)
{
    setWindowTitle("Login / Registrazione");

    auto *form = new QFormLayout;
    m_nameEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    form->addRow("Nome", m_nameEdit);
    form->addRow("Email", m_emailEdit);
    form->addRow("Password", m_passwordEdit);

    auto *okBtn = new QPushButton("OK", this);
    auto *cancelBtn = new QPushButton("Annulla", this);

    connect(okBtn, &QPushButton::clicked, this, &UserAuthDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &UserAuthDialog::reject);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnLayout);
}

void UserAuthDialog::onAccept()
{
    const QString name = m_nameEdit->text().trimmed();
    const QString email = m_emailEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (name.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Errore", "Nome ed email sono obbligatori.");
        return;
    }

    if (!m_db.isOpen()) {
        QMessageBox::warning(this, "Errore", "Database non aperto.");
        return;
    }

    m_result.name = name;
    m_result.email = email;
    m_result.password = password;

    accept();
}

AuthResult UserAuthDialog::result() const
{
    return m_result;
}
