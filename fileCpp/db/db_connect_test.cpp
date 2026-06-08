#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <iostream>
#include <string>
#include "db_connector.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::string host, user, password, dbname;
    int port = 3306;

    std::cout << "IP: ";
    std::getline(std::cin, host);
    std::cout << "Porta: ";
    std::string portStr;
    std::getline(std::cin, portStr);
    try { port = std::stoi(portStr); } catch(...) { }
    std::cout << "Username: ";
    std::getline(std::cin, user);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    std::cout << "Database: ";
    std::getline(std::cin, dbname);

    QSqlDatabase db;
    QString error;
    bool ok = DBConnector::connect(QString::fromStdString(host), port, QString::fromStdString(user), QString::fromStdString(password), QString::fromStdString(dbname), db, error);
    if (!ok) {
        std::cerr << "Connessione fallita: " << error.toStdString() << std::endl;
        return 1;
    }
    std::cout << "Connessione avvenuta con successo." << std::endl;
    db.close();
    QSqlDatabase::removeDatabase("connector_connection");
    return 0;
}
