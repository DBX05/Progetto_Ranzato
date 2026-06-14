#ifndef DATANASCITA_H
#define DATANASCITA_H

#include "dataora.h"
#include <string>

class dataNascita : public data, public mese, public anno
{
public:
    dataNascita() = delete;
    dataNascita(int giorno, int meseVal, int annoVal);
    explicit dataNascita(const std::string& dateString);

    std::string getDataFormatted() const;
    int getEta() const;
    bool isValidBirthDate() const;

    friend bool operator==(const dataNascita &x, const dataNascita &y);
    friend bool operator>(const dataNascita &x, const dataNascita &y);
    friend bool operator<(const dataNascita &x, const dataNascita &y);
};

#endif // DATANASCITA_H
