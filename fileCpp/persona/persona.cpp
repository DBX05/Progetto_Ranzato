#include "persona.h"
#include <regex>
#include <sstream>
#include <iomanip>
#include <cstring>

// ============================================================================
// PersonaException - Implementazione
// ============================================================================

/*!
    \fn PersonaException::PersonaException(const std::string& msg)
    \brief Costruisce un'eccezione con messaggio descrittivo.
    \param msg Messaggio descrittivo dell'errore.
*/
PersonaException::PersonaException(const std::string& msg) : message(msg) {}

/*!
    \fn const char* PersonaException::what() const noexcept
    \brief Restituisce il messaggio descrittivo dell'eccezione.
    \return Stringa C con il messaggio d'errore.
*/
const char* PersonaException::what() const noexcept
{
    return message.c_str();
}

// ============================================================================
// utente - Implementazione
// ============================================================================

/*!
    \fn utente::utente(unsigned int userId)
    \brief Costruisce un utente base con ID e policy di default.
    \param userId ID univoco dell'utente.
*/
utente::utente(unsigned int userId) : id(userId), policy(false) {}

/*!
    \fn unsigned int utente::getId() const
    \brief Restituisce l'ID univoco dell'utente.
    \return ID dell'utente.
*/
unsigned int utente::getId() const
{
    return id;
}

/*!
    \fn bool utente::getPolicyStatus() const
    \brief Restituisce lo stato di accettazione delle policy.
    \return \c true se le policy sono accettate, \c false altrimenti.
*/
bool utente::getPolicyStatus() const
{
    return policy;
}

/*!
    \fn void utente::setPolicyStatus(bool accepted)
    \brief Imposta lo stato di accettazione delle policy.
    \param accepted \c true per accettare, \c false per rifiutare.
*/
void utente::setPolicyStatus(bool accepted)
{
    policy = accepted;
}

// ============================================================================
// persona - Implementazione privata (Validazione)
// ============================================================================

/*!
    \fn bool persona::isValidEmail(const std::string& email)
    \brief Valida il formato di un'email.
    \param email Email da validare.
    \return \c true se l'email ha un formato valido, \c false altrimenti.
    
    Controlla che l'email rispetti il pattern: user@domain.extension
    con lunghezza tra 5 e 254 caratteri.
*/
bool persona::isValidEmail(const std::string& email)
{
    if (email.empty() || email.length() > 254 || email.length() < 5)
        return false;
    
    // Pattern regex per email: user@domain.extension
    static const std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

/*!
    \fn bool persona::isValidPassword(const std::string& password)
    \brief Valida la complessità di una password.
    \param password Password da validare.
    \return \c true se la password soddisfa i requisiti, \c false altrimenti.
    
    Requisiti minimi:
    - Lunghezza: almeno 8 caratteri
    - Contiene almeno una lettera maiuscola
    - Contiene almeno una lettera minuscola
    - Contiene almeno una cifra
    - Contiene almeno un carattere speciale (!@#$%^&*)
*/
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

/*!
    \fn bool persona::isValidDate(const std::string& date)
    \brief Valida una data nel formato YYYY-MM-DD.
    \param date Data da validare.
    \return \c true se la data ha un formato e valore valido, \c false altrimenti.
    
    Verifica:
    - Formato: YYYY-MM-DD
    - Anno: tra 1900 e anno corrente
    - Mese: tra 01 e 12
    - Giorno: valido per il mese specificato (considera anni bisestili)
*/
bool persona::isValidDate(const std::string& date)
{
    if (date.length() != 10)
        return false;
    
    // Pattern: YYYY-MM-DD
    static const std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    if (!std::regex_match(date, dateRegex))
        return false;
    
    try
    {
        int anno = std::stoi(date.substr(0, 4));
        int mese = std::stoi(date.substr(5, 2));
        int giorno = std::stoi(date.substr(8, 2));
        
        // Validazione anno: tra 1900 e anno corrente
        if (anno < 1900 || anno > 2026)
            return false;
        
        // Validazione mese
        if (mese < 1 || mese > 12)
            return false;
        
        // Validazione giorno per mese
        int giorniMese[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
        // Controllo anno bisestile
        if (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0))
            giorniMese[1] = 29;
        
        if (giorno < 1 || giorno > giorniMese[mese - 1])
            return false;
        
        return true;
    }
    catch (...)
    {
        return false;
    }
}

/*!
    \fn std::string persona::hashPassword(const std::string& password)
    \brief Calcola un hash semplice della password.
    \param password Password da hashare.
    \return Stringa contenente l'hash della password.
    
    \warning Questa è un'implementazione semplificata per scopi didattici.
    Per applicazioni in produzione, usare librerie crittografiche mature
    come bcrypt, scrypt o Argon2.
    
    Attualmente usa un semplice algoritmo basato su XOR con salt.
*/
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

/*!
    \fn persona::persona(unsigned int userId, const std::string& email, 
                          const std::string& password, const std::string& nome, 
                          const std::string& dataNascita)
    \brief Costruisce un oggetto persona con validazione completa.
    \param userId ID univoco dell'utente.
    \param email Email dell'utente (deve essere valida).
    \param password Password dell'utente (deve soddisfare i requisiti).
    \param nome Nome completo dell'utente (non vuoto).
    \param dataNascita Data di nascita (formato YYYY-MM-DD).
    
    \throws persona::InvalidEmailException Se l'email non è valida.
    \throws persona::InvalidPasswordException Se la password non soddisfa i requisiti.
    \throws persona::InvalidDateException Se la data non è valida.
    \throws std::invalid_argument Se il nome è vuoto.
*/
persona::persona(unsigned int userId, const std::string& email, 
                 const std::string& password, const std::string& nome, 
                 const std::string& dataNascita)
    : utente(userId)
{
    if (!isValidEmail(email))
        throw InvalidEmailException(email);
    
    if (!isValidPassword(password))
        throw InvalidPasswordException("La password deve contenere almeno 8 caratteri, "
                                      "una maiuscola, una minuscola, una cifra e un carattere speciale (!@#$%^&*)");
    
    if (nome.empty())
        throw std::invalid_argument("Il nome non può essere vuoto");
    
    if (!isValidDate(dataNascita))
        throw InvalidDateException(dataNascita);
    
    this->email = email;
    this->passwordHash = hashPassword(password);
    this->nome = nome;
    this->dataNascita = dataNascita;
}

/*!
    \fn std::string persona::getEmail() const
    \brief Restituisce l'email dell'utente.
    \return Email dell'utente.
*/
std::string persona::getEmail() const
{
    return email;
}

/*!
    \fn std::string persona::getNome() const
    \brief Restituisce il nome dell'utente.
    \return Nome completo dell'utente.
*/
std::string persona::getNome() const
{
    return nome;
}

/*!
    \fn std::string persona::getDataNascita() const
    \brief Restituisce la data di nascita dell'utente.
    \return Data di nascita nel formato YYYY-MM-DD.
*/
std::string persona::getDataNascita() const
{
    return dataNascita;
}

/*!
    \fn void persona::setEmail(const std::string& newEmail)
    \brief Modifica l'email dell'utente con validazione.
    \param newEmail Nuova email (deve essere valida).
    
    \throws persona::InvalidEmailException Se l'email non è valida.
*/
void persona::setEmail(const std::string& newEmail)
{
    if (!isValidEmail(newEmail))
        throw InvalidEmailException(newEmail);
    email = newEmail;
}

/*!
    \fn void persona::setPassword(const std::string& newPassword)
    \brief Modifica la password dell'utente con validazione.
    \param newPassword Nuova password (deve soddisfare i requisiti).
    
    \throws persona::InvalidPasswordException Se la password non soddisfa i requisiti.
*/
void persona::setPassword(const std::string& newPassword)
{
    if (!isValidPassword(newPassword))
        throw InvalidPasswordException("La password deve contenere almeno 8 caratteri, "
                                      "una maiuscola, una minuscola, una cifra e un carattere speciale (!@#$%^&*)");
    passwordHash = hashPassword(newPassword);
}

/*!
    \fn void persona::setNome(const std::string& newNome)
    \brief Modifica il nome dell'utente.
    \param newNome Nuovo nome (non può essere vuoto).
    
    \throws std::invalid_argument Se il nome è vuoto.
*/
void persona::setNome(const std::string& newNome)
{
    if (newNome.empty())
        throw std::invalid_argument("Il nome non può essere vuoto");
    nome = newNome;
}

/*!
    \fn void persona::setDataNascita(const std::string& newData)
    \brief Modifica la data di nascita dell'utente.
    \param newData Nuova data di nascita (formato YYYY-MM-DD).
    
    \throws persona::InvalidDateException Se la data non è valida.
*/
void persona::setDataNascita(const std::string& newData)
{
    if (!isValidDate(newData))
        throw InvalidDateException(newData);
    dataNascita = newData;
}

/*!
    \fn bool persona::verifyPassword(const std::string& password) const
    \brief Verifica se la password fornita corrisponde al record memorizzato.
    \param password Password da verificare.
    \return \c true se la password è corretta, \c false altrimenti.
    
    Questo metodo dovrebbe essere usato per l'autenticazione dell'utente.
*/
bool persona::verifyPassword(const std::string& password) const
{
    return hashPassword(password) == passwordHash;
}

/*!
    \fn void persona::updateProfile(const std::string& email, 
                                     const std::string& nome, 
                                     const std::string& dataNascita)
    \brief Aggiorna più campi della persona in una sola chiamata.
    \param email Nuova email (lasciare vuoto per non modificare).
    \param nome Nuovo nome (lasciare vuoto per non modificare).
    \param dataNascita Nuova data di nascita (lasciare vuoto per non modificare).
    
    \throws persona::InvalidEmailException Se la nuova email non è valida.
    \throws persona::InvalidDateException Se la nuova data non è valida.
    \throws std::invalid_argument Se il nuovo nome è vuoto (quando fornito).
    
    Questo metodo è particolarmente utile per i widget Qt di modifica profilo,
    poiché permette di validare tutti i campi prima di applicare le modifiche.
*/
void persona::updateProfile(const std::string& email, 
                            const std::string& nome, 
                            const std::string& dataNascita)
{
    // Validazione prima di applicare modifiche
    if (!email.empty() && !isValidEmail(email))
        throw InvalidEmailException(email);
    
    if (!nome.empty() && nome.length() == 0)
        throw std::invalid_argument("Il nome non può essere vuoto");
    
    if (!dataNascita.empty() && !isValidDate(dataNascita))
        throw InvalidDateException(dataNascita);
    
    // Applicazione delle modifiche
    if (!email.empty())
        this->email = email;
    
    if (!nome.empty())
        this->nome = nome;
    
    if (!dataNascita.empty())
        this->dataNascita = dataNascita;
}

/*!
    \fn void persona::gestionePolicy()
    \brief Implementazione del metodo astratto della classe base.
    
    \note Attualmente implementato come stub. Dovrebbe mostrare
    le policy all'utente e registrare l'accettazione.
*/
void persona::gestionePolicy()
{
    // TODO: Implementare gestione policy con UI Qt
}

// ============================================================================
// Exception classes - Implementazioni
// ============================================================================

/*!
    \fn persona::InvalidEmailException::InvalidEmailException(const std::string& email)
    \brief Costruisce un'eccezione per email non valida.
    \param email Email che ha causato l'errore.
*/
persona::InvalidEmailException::InvalidEmailException(const std::string& email)
    : PersonaException("Email non valida: " + email) {}

/*!
    \fn persona::InvalidPasswordException::InvalidPasswordException(const std::string& reason)
    \brief Costruisce un'eccezione per password non valida.
    \param reason Motivo del rifiuto della password.
*/
persona::InvalidPasswordException::InvalidPasswordException(const std::string& reason)
    : PersonaException("Password non valida: " + reason) {}

/*!
    \fn persona::InvalidDateException::InvalidDateException(const std::string& date)
    \brief Costruisce un'eccezione per data non valida.
    \param date Data che ha causato l'errore.
*/
persona::InvalidDateException::InvalidDateException(const std::string& date)
    : PersonaException("Data non valida: " + date) {}
