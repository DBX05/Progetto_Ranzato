#include "UserAuthDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include "UserAuthDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

UserAuthDialog::UserAuthDialog(const QSqlDatabase& db, QWidget* parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle("Login / Registrazione");

    auto* form = new QFormLayout;          // <-- QFormLayout corretto

    m_nameEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    form->addRow(tr("Nome"), m_nameEdit);
    form->addRow(tr("Email"), m_emailEdit);
    form->addRow(tr("Password"), m_passwordEdit);

    auto* okBtn = new QPushButton(tr("OK"), this);
    auto* cancelBtn = new QPushButton(tr("Annulla"), this);

    connect(okBtn, &QPushButton::clicked, this, &UserAuthDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &UserAuthDialog::reject);

    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
}

void UserAuthDialog::onAccept()
{
    QString name = m_nameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Inserisci il nome."));
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Inserisci l'email."));
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Inserisci la password."));
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
