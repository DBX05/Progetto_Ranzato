
#include <persona.h>
#include <dataora.h>
#include <dataora.cpp>
#include <dataNascita.cpp>
#include <string>
#include <regex>
#include <sstream>
#include <iomanip>

// ============================================================================
// PersonaException - Implementazione
// ============================================================================

PersonaException::PersonaException(const std::string& msg) : message(msg) {}

const std::string PersonaException::errore() const noexcept { return message; }

// ============================================================================
// utente - Implementazione
// ============================================================================

utente::utente(const int& userId): id(userId), policy(false) { }

const int utente::getId() const { return id; }

bool utente::getPolicyStatus() const { return policy; }

void utente::setPolicyStatus(bool accepted) { policy = accepted; }

// ============================================================================
// persona - Implementazione privata (Validazione)
// ============================================================================

bool persona::isValidEmail(const std::string& email)
{
    if (email.empty() || email.length() > 254 || email.length() < 5)
        return false;
    
    // Pattern regex per email: user@domain.extension
    const std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool persona::isValidPassword(const std::string& password)
{
    if (password.length() < 8)
        return false;
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    
    for (char c : password)
    {
        if (c >= 'A' && c <= 'Z') hasUpper = true;
        else if (c >= 'a' && c <= 'z') hasLower = true;
        else if (c >= '0' && c <= '9') hasDigit = true;
        else if (std::string("!@#$%^&*").find(c) != std::string::npos) hasSpecial = true;
    }
    
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

bool persona::isValidDateString(const std::string& date)
{
    if (date.length() != 10)
        return false;
    
    // Pattern: YYYY-MM-DD
    const std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    if (!std::regex_match(date, dateRegex))
        return false;
    
    try
    {
        int anno = std::stoi(date.substr(0, 4));
        int meseVal = std::stoi(date.substr(5, 2));
        int giorno = std::stoi(date.substr(8, 2));
        
        // Validazione anno: tra 1900 e anno corrente
        if (anno < 1900 || anno > 2026)
            return false;
        
        // Validazione mese
        if (meseVal < 1 || meseVal > 12)
            return false;
        
        // Validazione giorno per mese
        int giorniMese[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
        // Controllo anno bisestile
        if (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0))
            giorniMese[1] = 29;
        
        if (giorno < 1 || giorno > giorniMese[meseVal - 1])
            return false;
        
        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::string persona::hashPassword(const std::string& password)
{
    // Salt semplice (in produzione, usare salt random per ogni password)
    const std::string salt = "ProgettoBruzzi2026";
    
    // Algoritmo semplificato: concatenazione + XOR
    std::string toHash = password + salt;
    std::stringstream hashStream;
    
    unsigned long hash = 5381;
    for (char c : toHash)
    {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    
    hashStream << std::hex << hash;
    return hashStream.str();
}

// ============================================================================
// persona - Implementazione pubblica
// ============================================================================

explicit persona::persona(const int& userId, const std::string& email, 
                 const std::string& password, const std::string& nome, 
                 const dataNascita& nas)
    : utente(userId), nascita(nas)
{
    if (!isValidEmail(email)) throw InvalidEmailException(email);
    
    if (!isValidPassword(password)) throw InvalidPasswordException("La password deve contenere almeno 8 caratteri, "
                                      "una maiuscola, una minuscola, una cifra e un carattere speciale (!@#$%^&*)");
    
    if (nome.empty()) throw std::invalid_argument("Il nome non può essere vuoto");
    
    if (!nas.isValidBirthDate()) throw std::invalid_argument("La data di nascita non è valida");
    
    this->email = email;
    this->passwordHash = hashPassword(password);
    this->nome = nome;
}

std::string persona::getEmail() const { return email; }

std::string persona::getNome() const { return nome; }

dataNascita persona::getDataNascita() const { return nascita; }

std::string persona::getDataNascitaFormatted() const { return nascita.getDataFormatted(); }

int persona::getEta() const { return nascita.getEta(); }

void persona::setEmail(const std::string& newEmail)
{
    if (!isValidEmail(newEmail))
        throw InvalidEmailException(newEmail);
    email = newEmail;
}

void persona::setPassword(const std::string& newPassword)
{
    if (!isValidPassword(newPassword))
        throw InvalidPasswordException("La password deve contenere almeno 8 caratteri, "
                                      "una maiuscola, una minuscola, una cifra e un carattere speciale (!@#$%^&*)");
    passwordHash = hashPassword(newPassword);
}

void persona::setNome(const std::string& newNome)
{
    if (newNome.empty())
        throw std::invalid_argument("Il nome non può essere vuoto");
    nome = newNome;
}

void persona::setDataNascita(const dataNascita& newData)
{
    if (!newData.isValidBirthDate())
        throw std::invalid_argument("La data di nascita non è valida");
    nascita = newData;
}

bool persona::verifyPassword(const std::string& password) const { return hashPassword(password) == passwordHash; }

void persona::updateProfile(const std::string& email, 
                            const std::string& nome, 
                            const dataNascita* dataNas)
{
    // Validazione prima di applicare modifiche
    if (!email.empty() && !isValidEmail(email))
        throw InvalidEmailException(email);
    
    if (!nome.empty() && nome.length() == 0)
        throw std::invalid_argument("Il nome non può essere vuoto");
    
    if (dataNas != nullptr && !dataNas->isValidBirthDate())
        throw std::invalid_argument("La data di nascita non è valida");
    
    // Applicazione delle modifiche
    if (!email.empty())
        this->email = email;
    
    if (!nome.empty())
        this->nome = nome;
    
    if (dataNas != nullptr)
        this->nascita = *dataNas;
}

void persona::gestionePolicy()
{
    // TODO: Implementare gestione policy con UI Qt
}

// ============================================================================
// Exception classes - Implementazioni
// ============================================================================

persona::InvalidEmailException::InvalidEmailException(const std::string& email): PersonaException("Email non valida: " + email) {}

persona::InvalidPasswordException::InvalidPasswordException(const std::string& reason): PersonaException("Password non valida: " + reason) {}