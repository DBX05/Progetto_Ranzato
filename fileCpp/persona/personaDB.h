#ifndef PERSONADB_H
#define PERSONADB_H

#include "persona.h"
#include <string>
#include <memory>

// Forward declaration per il database
class DatabaseConnection;

/*
 * CLASSE: PersonaDatabase
 * Descrizione: Gestisce le operazioni di database per la classe persona
 * 
 * Funzionalità:
 *   - Verifica se un ID utente esiste nel database
 *   - Inserisce una nuova persona nel database
 *   - Carica i dati di una persona dal database
 *   - Aggiorna i dati di una persona nel database
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string connectionString: stringa di connessione al database
 *   - std::unique_ptr<DatabaseConnection> dbConnection: connessione al DB
 */
class PersonaDatabase
{
private:
    std::string dbHost;
    std::string dbUser;
    std::string dbPassword;
    std::string dbName;

    // Inizializza la connessione al database
    // Input: nessuno
    // Output: stabilisce connessione MySQL
    // Eccezioni: std::runtime_error se connessione fallisce
    void initializeConnection();

public:
    // Costruttore: inizializza con parametri di connessione
    // Input: const string& host, const string& user, const string& password, const string& database
    // Output: oggetto PersonaDatabase inizializzato
    PersonaDatabase(const std::string& host = "localhost",
                    const std::string& user = "root",
                    const std::string& password = "",
                    const std::string& database = "AgendaDB");

    // Distruttore
    ~PersonaDatabase();

    // Verifica se un ID utente esiste nel database
    // Input: unsigned int userId
    // Output: bool (true=esiste, false=non esiste)
    // Eccezioni: std::runtime_error se query fallisce
    bool userExists(unsigned int userId);

    // Inserisce una nuova persona nel database
    // Input: const persona& p
    // Output: true se inserimento riuscito
    // Eccezioni: std::runtime_error se query fallisce
    // Nota: l'ID della persona viene generato automaticamente dal database
    bool insertPersona(const persona& p);

    // Carica i dati di una persona dal database
    // Input: unsigned int userId
    // Output: oggetto persona caricato dal database
    // Eccezioni: std::runtime_error se query fallisce o ID non esiste
    // Nota: Restituisce una nuova persona con dati dal DB
    persona loadPersona(unsigned int userId);

    // Aggiorna i dati di una persona nel database
    // Input: const persona& p
    // Output: bool (true=aggiornamento riuscito)
    // Eccezioni: std::runtime_error se query fallisce
    bool updatePersona(const persona& p);

    // Elimina una persona dal database
    // Input: unsigned int userId
    // Output: bool (true=eliminazione riuscita)
    // Eccezioni: std::runtime_error se query fallisce
    bool deletePersona(unsigned int userId);

    // Eccezione specifica per errori di database
    class DatabaseException : public PersonaException
    {
    public:
        explicit DatabaseException(const std::string& message);
    };
};

#endif // PERSONADB_H
