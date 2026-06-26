#ifndef DBCONNECTDIALOG_H
#define DBCONNECTDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;

struct DBConnectParams {
    QString host;
    int port;
    QString user;
    QString password;
    QString dbName;
};

class DBConnectDialog : public QDialog {
    Q_OBJECT
public:
    explicit DBConnectDialog(QWidget* parent = nullptr);
    DBConnectParams params() const;
    QSqlDatabase connectedDb() const;

private slots:
    void validateInputs();
    void tryConnect();
    void updateInputEnableState();

private:
    QLineEdit* m_host;
    QSpinBox* m_port;
    QLineEdit* m_user;
    QLineEdit* m_password;
    QLineEdit* m_dbName;
    QPushButton* m_connectButton;
    QPushButton* m_cancelButton;
    QLabel* m_errorLabel;
    QSqlDatabase m_connectedDb;
};

#endif // DBCONNECTDIALOG_H
