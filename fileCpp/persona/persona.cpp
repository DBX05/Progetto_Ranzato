#include "persona.h"
#include <iostream>

// DEFINIZIONE METODI PERSONA
void persona::setPassword(const std::string& newPassword) { password = newPassword; }
persona::persona(std::string em, std::string psw, std::string no, std::string dataNas): email(em), nome(no), dataNascita(dataNas) { setPassword(psw); }
std::string persona::getEmail() const { return persona::email; }
std::string persona::getPassowrd() const { return persona::password; }
std::string persona::getNome() const { return persona::nome; }
std::string persona::getDataNascita() const { return persona::dataNascita; }

void persona::modificaPersona(int bitModEmail, int bitModPassword, int bitModNome, int bitModDataNascita)
{
    if ( bitModEmail == 1)
    {
        std::string temp;
        std::cout << "Inserire nuova Email: ";
        std::cin >> temp;
        persona::email = temp;

    }
    if ( bitModPassword == 1)
    {
        std::string temp;
        std::cout << "Inserire nuova Password: ";
        std::cin >> temp;
        setPassword(temp);
    }
    if ( bitModNome == 1)
    {
        std::string temp;
        std::cout << "Inserire nuovo Nome: ";
        std::cin >> temp;
        persona::nome = temp;
    }
    if ( bitModDataNascita == 1)
    {
        std::string temp;
        std::cout << "Inserire nuova data di nascita: ";
        std::cin >> temp;
        persona::dataNascita = temp;
    }
}