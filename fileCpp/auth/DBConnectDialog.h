#ifndef DBCONNECTDIALOG_H
#define DBCONNECTDIALOG_H

#include <QDialog>

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

private slots:
    void validateInputs();

private:
    QLineEdit* m_host;
    QSpinBox* m_port;
    QLineEdit* m_user;
    QLineEdit* m_password;
    QLineEdit* m_dbName;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    QLabel* m_errorLabel;
};

#endif // DBCONNECTDIALOG_H
