/*#include "AuthUtils.h"
#include "DButils.h"

AuthResult authenticateUser(MYSQL* conn, const std::string& name, const std::string& email, const std::string& password)
{
    AuthResult r;
    if (!conn) return r;

    int uid = ensureUserExists(conn, name, email, password);
    if (uid >= 0) {
        r.ok = true;
        r.name = name;
        r.email = email;
        r.userId = uid;
    }
    return r;
}
  */