#ifndef PERSONA_H
#define PERSONA_H

#include <string>
#include <cstdint>
#include <stdexcept>

class dataNascita;  // Forward declaration

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
    
    Questa classe gestisce i dati personali dell'utente:
    - Email (con validazione formato)
    - Password (con hash)
    - Nome
    - Data di nascita (usando la classe dataNascita)
*/
class persona : public utente
{
private:
    std::string email;
    std::string passwordHash;
    std::string nome;
    dataNascita dataNascita_member;
    
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    static bool isValidDateString(const std::string& date);
    static std::string hashPassword(const std::string& password);
    
public:
    persona() = delete;
    explicit persona(unsigned int userId, const std::string& email, 
                     const std::string& password, const std::string& nome, 
                     const dataNascita& dataNas);
    
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
                       const dataNascita* dataNas = nullptr);
    
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
};

#endif // PERSONA_H
