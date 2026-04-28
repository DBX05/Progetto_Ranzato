#ifndef PERSONA_H
#define PERSONA_H

#include <string>
#include <cstdint>
#include <stdexcept>

/*!
    \class PersonaException
    \brief Eccezione base per errori di gestione persona.
*/
class PersonaException : public std::exception
{
private:
    std::string message;
public:
    PersonaException(const std::string& msg);
    const char* what() const noexcept override;
};

/*!
    \class utente
    \brief Classe base astratta per utenti del sistema.
*/
class utente
{
protected:
    unsigned int id;
    bool policy;
    
    utente(unsigned int userId);
    
public:
    virtual ~utente() = default;
    
    unsigned int getId() const;
    bool getPolicyStatus() const;
    void setPolicyStatus(bool accepted);
    virtual void gestionePolicy() = 0;
};

/*!
    \class persona
    \brief Gestione profilo utente con validazione e controllo errori.
*/
class persona : public utente
{
private:
    std::string email;
    std::string passwordHash;
    std::string nome;
    std::string dataNascita;
    
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    static bool isValidDate(const std::string& date);
    static std::string hashPassword(const std::string& password);
    
public:
    persona() = delete;
    explicit persona(unsigned int userId, const std::string& email, 
                     const std::string& password, const std::string& nome, 
                     const std::string& dataNascita);
    
    std::string getEmail() const;
    std::string getNome() const;
    std::string getDataNascita() const;
    
    void setEmail(const std::string& newEmail);
    void setPassword(const std::string& newPassword);
    void setNome(const std::string& newNome);
    void setDataNascita(const std::string& newData);
    
    bool verifyPassword(const std::string& password) const;
    
    void updateProfile(const std::string& email = "", 
                       const std::string& nome = "", 
                       const std::string& dataNascita = "");
    
    void gestionePolicy() override;
    
    /*!
        \class InvalidEmailException
        \brief Eccezione per email non valida.
    */
    class InvalidEmailException : public PersonaException
    {
    public:
        InvalidEmailException(const std::string& email);
    };
    
    /*!
        \class InvalidPasswordException
        \brief Eccezione per password non valida.
    */
    class InvalidPasswordException : public PersonaException
    {
    public:
        InvalidPasswordException(const std::string& reason);
    };
    
    /*!
        \class InvalidDateException
        \brief Eccezione per data non valida.
    */
    class InvalidDateException : public PersonaException
    {
    public:
        InvalidDateException(const std::string& date);
    };
};

#endif // PERSONA_H
