#ifndef PERSONA_H
#define PERSONA_H

#include <string>
#include <cstdint>
#include <stdexcept>

class dataNascita;  // Forward declaration

/*
 * CLASSE: PersonaException
 * Descrizione: Eccezione base per errori di gestione persona
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string message: messaggio descrittivo dell'errore
 * 
 * METODI:
 *   - PersonaException(string): costruttore
 *   - what(): restituisce il messaggio d'errore
 */
class PersonaException : public std::exception
{
private:
    std::string message;

public:
    // Costruttore: crea eccezione con messaggio descrittivo
    // Input: const string& msg
    // Output: oggetto eccezione inizializzato
    explicit PersonaException(const std::string& msg);

    // Getter: restituisce il messaggio descrittivo dell'eccezione
    // Input: nessuno
    // Output: string messaggio d'errore
    const std::string errore() const noexcept;
}; // fine classe PersonaException

/*
 * CLASSE: utente
 * Descrizione: Classe astratta base per utenti del sistema
 * 
 * ATTRIBUTI PROTECTED:
 *   - const int id: identificativo univoco dell'utente (costante)
 *   - bool policy: stato di accettazione delle policy
 * 
 * METODI:
 *   - utente() costruttore di default
 *   - utente(const int&): costruttore con valore
 *   - virtual ~utente(): distruttore virtuale
 *   - getId(): restituisce ID
 *   - getPolicyStatus(): restituisce stato policy
 *   - setPolicyStatus(bool): imposta stato policy
 *   - virtual gestionePolicy(): metodo astratto
 */
class utente
{
private:
    const int id;
    bool policy;
 
public:
    // costruttore di default
    utente() = default;

    // costruttore con costruzione dei campi dato
    utente(const int&);

    // Distruttore virtuale
    virtual ~utente();

    // Getter: restituisce l'ID univoco dell'utente
    // Input: nessuno
    // Output: unsigned int id
    const int getId() const;

    // Getter: restituisce lo stato di accettazione delle policy
    // Input: nessuno
    // Output: bool (true=accettate, false=rifiutate)
    bool getPolicyStatus() const;

    // Imposta lo stato di accettazione delle policy
    // Input: bool accepted (true=accetta, false=rifiuta)
    // Output: aggiorna policy
    void setPolicyStatus(bool accepted);

    // Gestione delle policy dell'utente (metodo astratto puro)
    // Input: nessuno
    // Output: implementata dalle sottoclassi
    virtual void gestionePolicy() = 0;
}; // fine classe Utente

/*
 * CLASSE: persona
 * Descrizione: Gestione profilo utente con validazione e controllo errori
 * Estende: utente
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string email: indirizzo email (validato)
 *   - std::string passwordHash: hash della password
 *   - std::string nome: nome completo utente
 *   - dataNascita dataNascita_member: data di nascita
 * 
 * METODI STATIC PRIVATI (validazione):
 *   - isValidEmail(string): valida formato email
 *   - isValidPassword(string): valida complessità password
 *   - isValidDateString(string): valida formato data
 *   - hashPassword(string): calcola hash password
 * 
 * METODI PUBBLICI:
 *   - persona(...): costruttore
 *   - getEmail/getNome/getDataNascita/getEta: getter
 *   - setEmail/setPassword/setNome/setDataNascita: setter con validazione
 *   - verifyPassword: verifica password
 *   - updateProfile: aggiorna più campi
 *   - gestionePolicy: override metodo astratto
 */
class persona : public utente
{
private:
    std::string email;
    std::string passwordHash;
    std::string nome;
    dataNascita dataNascita_member;

    // Valida il formato di un'email
    // Input: const string& email
    // Output: bool (true=valida, false=non valida)
    // Valida: pattern user@domain.extension, lunghezza 5-254 caratteri
    static bool isValidEmail(const std::string& email);

    // Valida la complessità di una password
    // Input: const string& password
    // Output: bool (true=valida, false=non valida)
    // Requisiti: min 8 caratteri, maiuscola, minuscola, cifra, carattere speciale
    static bool isValidPassword(const std::string& password);

    // Valida una data nel formato YYYY-MM-DD
    // Input: const string& date
    // Output: bool (true=valida, false=non valida)
    // Valida: formato YYYY-MM-DD, anno 1900-2026, mese 01-12, giorno valido per mese
    static bool isValidDateString(const std::string& date);

    // Calcola l'hash della password
    // Input: const string& password
    // Output: string contenente l'hash
    static std::string hashPassword(const std::string& password);

public:
    // Costruttore: inizializza persona con validazione completa di tutti i campi
    // Input: const int& userId, const string& email (validata), const string& password (validata),
    //        const string& nome (non vuoto), const dataNascita& dataNas (validata)
    // Output: oggetto persona inizializzato
    // Eccezioni: InvalidEmailException, InvalidPasswordException, invalid_argument
    explicit persona(const int& userId, const std::string& email,
                     const std::string& password, const std::string& nome,
                     const dataNascita& dataNas);

    // Getter: restituisce l'email dell'utente
    // Input: nessuno
    // Output: string email
    std::string getEmail() const;

    // Getter: restituisce il nome dell'utente
    // Input: nessuno
    // Output: string nome completo
    std::string getNome() const;

    // Getter: restituisce la data di nascita come oggetto dataNascita
    // Input: nessuno
    // Output: dataNascita oggetto data di nascita
    dataNascita getDataNascita() const;

    // Getter: restituisce la data di nascita formattata come stringa (GG/MM/AAAA)
    // Input: nessuno
    // Output: string data formattata
    std::string getDataNascitaFormatted() const;

    // Getter: calcola e restituisce l'eta corrente dell'utente
    // Input: nessuno
    // Output: int eta in anni
    int getEta() const;

    // Modifica l'email con validazione
    // Input: const string& newEmail (deve essere valida)
    // Output: aggiorna email se valida
    // Eccezioni: InvalidEmailException se email non valida
    void setEmail(const std::string& newEmail);

    // Modifica la password con validazione
    // Input: const string& newPassword (deve soddisfare requisiti)
    // Output: aggiorna passwordHash se valida
    // Eccezioni: InvalidPasswordException se non soddisfa requisiti
    void setPassword(const std::string& newPassword);

    // Modifica il nome
    // Input: const string& newNome (non vuoto)
    // Output: aggiorna nome se non vuoto
    // Eccezioni: invalid_argument se nome vuoto
    void setNome(const std::string& newNome);

    // Modifica la data di nascita
    // Input: const dataNascita& newData (data valida)
    // Output: aggiorna dataNascita_member se valida
    // Eccezioni: invalid_argument se data non valida
    void setDataNascita(const dataNascita& newData);

    // Verifica se la password fornita corrisponde al record memorizzato
    // Input: const string& password (da verificare)
    // Output: bool (true=corretta, false=errata)
    // Usato per autenticazione dell'utente
    bool verifyPassword(const std::string& password) const;

    // Aggiorna più campi della persona in una sola chiamata
    // Input: const string& email (vuoto=non modifica), const string& nome (vuoto=non modifica),
    //        const dataNascita* dataNas (nullptr=non modifica)
    // Output: aggiorna i campi non vuoti/non null
    // Eccezioni: InvalidEmailException, invalid_argument se dati non validi
    void updateProfile(const std::string& email = "",
                       const std::string& nome = "",
                       const dataNascita* dataNas = nullptr);

    // Implementazione del metodo astratto della classe base
    // Input: nessuno
    // Output: gestisce policy utente (da implementare con Qt)
    void gestionePolicy() override;

    /*
     * CLASSE ANNIDATA: InvalidEmailException
     * Descrizione: Eccezione specifica per email non valida
     * Estende: PersonaException
     */
    class InvalidEmailException : public PersonaException
    {
    public:
        // Costruttore: crea eccezione per email non valida
        // Input: const string& email (email che ha causato l'errore)
        // Output: eccezione con messaggio descrittivo
        explicit InvalidEmailException(const std::string& email);
    }; // fine classe InvalidEmailException

    /*
     * CLASSE ANNIDATA: InvalidPasswordException
     * Descrizione: Eccezione specifica per password non valida
     * Estende: PersonaException
     */
    class InvalidPasswordException : public PersonaException
    {
    public:
        // Costruttore: crea eccezione per password non valida
        // Input: const string& reason (motivo del rifiuto della password)
        // Output: eccezione con messaggio descrittivo
        explicit InvalidPasswordException(const std::string& reason);
    }; // fine classe InvalidPasswordException
}; // fine classe Persona

#endif // PERSONA_H