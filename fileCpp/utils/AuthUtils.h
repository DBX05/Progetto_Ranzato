/*#pragma once

#include <string>
#include <mysql.h>

struct AuthResult {
    bool ok = false;
    std::string name;
    std::string email;
    int userId = -1;
};

// Effettua login o registrazione minima usando la connessione MySQL.
// Restituisce AuthResult con ok=true se l'operazione è andata a buon fine.
AuthResult authenticateUser(MYSQL* conn, const std::string& name, const std::string& email, const std::string& password);
*/