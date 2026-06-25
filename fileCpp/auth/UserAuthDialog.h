// UserAuthDialog.h
#ifndef USERAUTHDIALOG_H
#define USERAUTHDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

class QLineEdit;

struct AuthResult {
    int userId = -1;
    QString name;
    QString email;
    QString password;
};

class UserAuthDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UserAuthDialog(const QSqlDatabase &db, QWidget *parent = nullptr);
    AuthResult result() const;

private slots:
    void onAccept();

private:
    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_emailEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;

    AuthResult m_result;
    QSqlDatabase m_db;   // COPIA, non riferimento
};

#endif
