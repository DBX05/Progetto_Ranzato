#ifndef PERSONA_H
#define PERSONA_H

#include <string>
#include <cstdint>
#include <stdexcept>
#include "../dataora/dataNascita.h"

class PersonaException : public std::exception
{
private:
    std::string message;

public:
    explicit PersonaException(const std::string& msg);
    const std::string errore() const noexcept;
};

// utente: rimuovere costruttore di default perché 'id' è const
class utente
{
private:
    const int id;
    bool policy;

public:
    utente() = delete;
    explicit utente(const int&);

    virtual ~utente();

    const int getId() const;
    bool getPolicyStatus() const;
    void setPolicyStatus(bool accepted);

    virtual void gestionePolicy() = 0;
};

class persona : public utente
{
private:
    std::string email;
    std::string passwordHash;
    std::string nome;
    dataNascita nascita;

    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    static bool isValidDateString(const std::string& date);
    static std::string hashPassword(const std::string& password);

public:
    explicit persona(const int& userId, const std::string& email,
                     const std::string& password, const std::string& nome,
                     const dataNascita& nascita);

    std::string getEmail() const;
    std::string getNome() const;
    dataNascita getDataNascita() const;
    std::string getDataNascitaFormatted() const;
    int getEta() const;

    void setEmail(const std::string& newEmail);
    void setPassword(const std::string& newPassword);
    void setNome(const std::string& newNome);
    void setDataNascita(const dataNascita& newData);

    bool verifyPassword(const std::string& password) const;
    void updateProfile(const std::string& email = "",
                       const std::string& nome = "",
                       const dataNascita* nas = nullptr);

    void gestionePolicy() override;

    class InvalidEmailException : public PersonaException
    {
    public:
        explicit InvalidEmailException(const std::string& email);
    };

    class InvalidPasswordException : public PersonaException
    {
    public:
        explicit InvalidPasswordException(const std::string& reason);
    };
};

#endif // PERSONA_H
