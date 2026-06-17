#ifndef USERAUTHDIALOG_H
#define USERAUTHDIALOG_H

#include <QDialog>
#include <memory>

class QLineEdit;
class QPushButton;
class QSqlDatabase;

struct AuthResult {
    int userId = -1;
    QString name;
    QString email;
};

class UserAuthDialog : public QDialog {
    Q_OBJECT
public:
    explicit UserAuthDialog(QSqlDatabase db, QWidget* parent = nullptr);

    // Se autenticazione ok, restituisce AuthResult con userId>0
    AuthResult result() const;

private:
    QLineEdit* m_name;
    QLineEdit* m_email;
    QLineEdit* m_password;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    AuthResult m_result;
    QSqlDatabase m_db;

    bool tryLogin();
    bool tryRegister();
    QString hashPassword(const QString& plain) const;
};

#endif // USERAUTHDIALOG_H
