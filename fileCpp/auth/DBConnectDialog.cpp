// DBConnectDialog.cpp
#include "DBConnectDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>

DBConnectDialog::DBConnectDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Connessione al Database");
    auto* layout = new QVBoxLayout(this);

    auto* hostLayout = new QHBoxLayout;
    hostLayout->addWidget(new QLabel("Host:"));
    m_host = new QLineEdit("localhost", this);
    hostLayout->addWidget(m_host);
    layout->addLayout(hostLayout);

    auto* portLayout = new QHBoxLayout;
    portLayout->addWidget(new QLabel("Port:"));
    m_port = new QSpinBox(this);
    m_port->setRange(0, 65535);
    m_port->setValue(3306); // default MySQL port
    portLayout->addWidget(m_port);
    layout->addLayout(portLayout);

    auto* userLayout = new QHBoxLayout;
    userLayout->addWidget(new QLabel("Username:"));
    m_user = new QLineEdit(this);
    userLayout->addWidget(m_user);
    layout->addLayout(userLayout);

    auto* passLayout = new QHBoxLayout;
    passLayout->addWidget(new QLabel("Password:"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    passLayout->addWidget(m_password);
    layout->addLayout(passLayout);

    auto* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(new QLabel("Database name / SQLite file:"));
    m_dbName = new QLineEdit(this);
    m_dbName->setText("agenda_db"); // suggerimento per MySQL
    dbLayout->addWidget(m_dbName);
    layout->addLayout(dbLayout);

    auto* btnLayout = new QHBoxLayout;
    m_ok = new QPushButton("Connetti", this);
    m_cancel = new QPushButton("Annulla", this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_ok);
    btnLayout->addWidget(m_cancel);
    layout->addLayout(btnLayout);

    connect(m_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancel, &QPushButton::clicked, this, &QDialog::reject);
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
