#include "dataNascita.h"
#include <regex>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdexcept>

dataNascita::dataNascita(int giorno, int meseVal, int annoVal)
    : data(giorno), mese(meseVal), anno(annoVal)
{
    // validazione già effettuata dalle classi base
}

dataNascita::dataNascita(const std::string& dateString)
{
    if (dateString.length() != 10 || dateString[2] != '/' || dateString[5] != '/')
        throw dataexp("Formato data non valido. Usare GG/MM/AAAA", -1);

    try
    {
        int giorno = std::stoi(dateString.substr(0, 2));
        int meseVal = std::stoi(dateString.substr(3, 2));
        int annoVal = std::stoi(dateString.substr(6, 4));

        data tempData(giorno);
        mese tempMese(meseVal - 1); // se i tuoi mesi in mese sono 0-based
        anno tempAnno(annoVal);

        *static_cast<data*>(this) = tempData;
        *static_cast<mese*>(this) = tempMese;
        *static_cast<anno*>(this) = tempAnno;
    }
    catch (const std::exception&)
    {
        throw dataexp("Errore nel parsing della data: " + dateString, -1);
    }
}

std::string dataNascita::getDataFormatted() const
{
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << (getDate()) << "/"
       << std::setw(2) << (numMese() + 1) << "/"
       << std::setw(4) << getAnno();
    return ss.str();
}

int dataNascita::getEta() const
{
    int annoCorrente = anno::systemYear();
    int annoBirth = getAnno();
    int eta = annoCorrente - annoBirth;

    int meseCorrente = mese::systemMonth() + 1;
    int meseNascita = numMese() + 1;

    if (meseCorrente < meseNascita)
        eta--;
    else if (meseCorrente == meseNascita)
    {
        int giornoCorrente = data::systemDay();
        int giornoNascita = getDate();
        if (giornoCorrente < giornoNascita)
            eta--;
    }

    return eta;
}

bool dataNascita::isValidBirthDate() const
{
    int annoCorrente = anno::systemYear();
    int annoBirth = getAnno();

    if (annoBirth > annoCorrente) return false;

    int eta = getEta();
    if (eta < 0 || eta > 150) return false;

    return true;
}

bool operator==(const dataNascita &x, const dataNascita &y)
{
    return (x.getDate() == y.getDate() &&
            x.numMese() == y.numMese() &&
            x.getAnno() == y.getAnno());
}

bool operator>(const dataNascita &x, const dataNascita &y)
{
    if (x.getAnno() != y.getAnno()) return x.getAnno() > y.getAnno();
    if (x.numMese() != y.numMese()) return x.numMese() > y.numMese();
    return x.getDate() > y.getDate();
}

bool operator<(const dataNascita &x, const dataNascita &y)
{
    if (x.getAnno() != y.getAnno()) return x.getAnno() < y.getAnno();
    if (x.numMese() != y.numMese()) return x.numMese() < y.numMese();
    return x.getDate() < y.getDate();
}
