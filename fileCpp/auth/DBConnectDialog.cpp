// DBConnectDialog.cpp
#include "DBConnectDialog.h"
#include "../db/db_connector.h" // Include the DBConnector header

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSpinBox> // For port number
#include <QDebug>

DBConnectDialog::DBConnectDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Connessione al Database");
    setModal(true); // Make the dialog modal

    auto* mainLayout = new QVBoxLayout(this);

    // --- Database Name / File Input ---
    auto* dbNameLayout = new QHBoxLayout;
    dbNameLayout->addWidget(new QLabel("Database name / SQLite file:"));
    m_dbName = new QLineEdit("agendadb.db", this); // Default value
    // Basic validation: not empty
    connect(m_dbName, &QLineEdit::textChanged, this, &DBConnectDialog::validateInputs);
    dbNameLayout->addWidget(m_dbName);
    mainLayout->addLayout(dbNameLayout);

    // --- Host Input (for non-SQLite databases like MySQL) ---
    auto* hostLayout = new QHBoxLayout;
    hostLayout->addWidget(new QLabel("Host (leave empty for SQLite):"));
    m_host = new QLineEdit(this);
    connect(m_host, &QLineEdit::textChanged, this, &DBConnectDialog::validateInputs);
    hostLayout->addWidget(m_host);
    mainLayout->addLayout(hostLayout);

    // --- Port Input ---
    auto* portLayout = new QHBoxLayout;
    portLayout->addWidget(new QLabel("Port:"));
    m_port = new QSpinBox(this);
    m_port->setRange(1, 65535); // Valid port range
    m_port->setValue(3306); // Default MySQL port
    m_port->setEnabled(false); // Initially disabled, enabled if host is set
    connect(m_port, QOverload<int>::of(&QSpinBox::valueChanged), this, &DBConnectDialog::validateInputs);
    portLayout->addWidget(m_port);
    portLayout->addStretch();
    mainLayout->addLayout(portLayout);

    // --- User Input ---
    auto* userLayout = new QHBoxLayout;
    userLayout->addWidget(new QLabel("User:"));
    m_user = new QLineEdit(this);
    m_user->setEnabled(false); // Initially disabled
    connect(m_user, &QLineEdit::textChanged, this, &DBConnectDialog::validateInputs);
    userLayout->addWidget(m_user);
    mainLayout->addLayout(userLayout);

    // --- Password Input ---
    auto* passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(new QLabel("Password:"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setEnabled(false); // Initially disabled
    passwordLayout->addWidget(m_password);
    mainLayout->addLayout(passwordLayout);

    // --- Error Message Label ---
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: #b00020;"); // Red color for errors
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setText(""); // Initially empty
    mainLayout->addWidget(m_errorLabel);

    // --- Button Layout ---
    auto* buttonLayout = new QHBoxLayout;
    m_cancelButton = new QPushButton("Cancel", this);
    m_connectButton = new QPushButton("Connect", this);
    m_connectButton->setDefault(true); // Make it the default button
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_connectButton);
    mainLayout->addLayout(buttonLayout);

    // --- Signal Connections ---
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_connectButton, &QPushButton::clicked, this, &DBConnectDialog::tryConnect);
    connect(m_host, &QLineEdit::textChanged, this, &DBConnectDialog::updateInputEnableState);
    connect(m_dbName, &QLineEdit::textChanged, this, &DBConnectDialog::validateInputs); // Re-validate on dbName change

    // Initial validation and state update
    validateInputs();
    updateInputEnableState();
}

// Try to establish the database connection using the provided parameters
void DBConnectDialog::tryConnect() {
    QString dbName = m_dbName->text().trimmed();
    QString host = m_host->text().trimmed();
    int port = m_port->value();
    QString user = m_user->text().trimmed();
    QString password = m_password->text(); // Password can be empty

    if (dbName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Database name or SQLite file path cannot be empty.");
        return;
    }

    QSqlDatabase db; // This will hold the connected database object
    QString connectionError;

    bool success = false;
    if (host.isEmpty()) {
        // SQLite connection
        qDebug() << "[DBConnectDialog] Attempting SQLite connection.";
        success = DBConnector::connect(dbName, db, connectionError);
    } else {
        // Non-SQLite connection (e.g., MySQL)
        if (user.isEmpty()) {
             QMessageBox::warning(this, "Input Error", "Username is required for non-SQLite databases.");
             return;
        }
         if (port <= 0) {
             QMessageBox::warning(this, "Input Error", "Port number must be greater than 0.");
             return;
        }
        qDebug() << "[DBConnectDialog] Attempting non-SQLite connection.";
        // NOTE: DBConnector currently only supports QSQLITE.
        // To support other databases, DBConnector::connect needs modification.
        // For now, we'll simulate a connection error or just use SQLite.
        // Example: For MySQL, you would use QSqlDatabase::addDatabase("QMYSQL");
        // and set host, port, user, password.
        // Since DBConnector currently only has QSQLITE, we'll treat this path as needing modification.
        connectionError = "Non-SQLite database connection is not yet implemented in DBConnector.";
        qWarning() << connectionError;
        // success = DBConnector::connect(host, port, user, password, dbName, db, connectionError); // Hypothetical call
    }

    if (success) {
        qDebug() << "Database connection successful!";
        // Store the database object in a way accessible by the main application
        // For simplicity here, we'll rely on the caller to retrieve it if needed,
        // but often this dialog would emit a signal with the DB object.
        m_connectedDb = db; // Store the successfully connected DB object
        accept(); // Close the dialog and return QDialog::Accepted
    } else {
        // Display the error message from DBConnector
        m_errorLabel->setText("Connection failed: " + connectionError);
        QMessageBox::warning(this, "Connection Error", connectionError);
    }
}

// Enable/disable input fields based on whether a host is specified (for non-SQLite)
void DBConnectDialog::updateInputEnableState() {
    bool useHost = !m_host->text().trimmed().isEmpty();
    m_port->setEnabled(useHost);
    m_user->setEnabled(useHost);
    m_password->setEnabled(useHost);

    // If not using host (SQLite), maybe reset user/password fields or leave them disabled
    if (!useHost) {
        // Optional: Clear fields or set defaults for SQLite
        // m_user->clear();
        // m_password->clear();
    }
    validateInputs(); // Re-validate after changing enable state
}

// Validate all input fields and update the Connect button state and error message
void DBConnectDialog::validateInputs() {
    QString dbName = m_dbName->text().trimmed();
    QString host = m_host->text().trimmed();
    int port = m_port->value();
    QString user = m_user->text().trimmed();

    bool hostSpecified = !host.isEmpty();
    bool inputsAreValid = true;
    QString errorMessage = "";

    // 1. Database Name / File Path Validation
    if (dbName.isEmpty()) {
        errorMessage = "Database name or SQLite file path is required.";
        inputsAreValid = false;
    } else if (hostSpecified) {
        // 2. Host Validation (if specified)
        // Basic check: host should have at least one non-space character.
        // More complex regex could be used for IP/domain validation.
        if (host.length() < 1) {
            errorMessage = "Invalid host format.";
            inputsAreValid = false;
        } else if (user.isEmpty()) {
            // 3. User Validation (required if host is specified)
            errorMessage = "Username is required when a host is specified.";
            inputsAreValid = false;
        } else if (port <= 0) {
            // 4. Port Validation (required if host is specified)
            errorMessage = "Port must be a valid number greater than 0.";
            inputsAreValid = false;
        }
    }

    // Update error label and button state
    m_errorLabel->setText(inputsAreValid ? "" : errorMessage);
    m_connectButton->setEnabled(inputsAreValid);
}

// Accessor for the connected database object after successful connection
QSqlDatabase DBConnectDialog::connectedDb() const {
    return m_connectedDb;
}

// Accessor for connection parameters (might be useful if not directly returning Db object)
DBConnectParams DBConnectDialog::params() const {
    DBConnectParams p;
    p.host = m_host->text();
    p.port = m_port->value();
    p.user = m_user->text();
    p.password = m_password->text();
    p.dbName = m_dbName->text();
    return p;
}
