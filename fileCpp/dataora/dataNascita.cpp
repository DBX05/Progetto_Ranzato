#include "dataora.h"
#include <regex>
#include <ctime>
#include <sstream>
#include <iomanip>

/*!
    \fn dataNascita::dataNascita(int giorno, int meseVal, int annoVal)
    \brief Costruisce una data di nascita con validazione.
    \param giorno Giorno della settimana (0-6, dove 0 è lunedì).
    \param meseVal Mese (0-11, dove 0 è gennaio).
    \param annoVal Anno (es. 1995).
    
    \throws dataexp Se il giorno non è valido.
    \throws mesexp Se il mese non è valido.
    \throws annoexp Se l'anno non è valido.
*/
dataNascita::dataNascita(int giorno, int meseVal, int annoVal)
    : data(giorno), mese(meseVal), anno(annoVal)
{
}

/*!
    \fn dataNascita::dataNascita(const std::string& dateString)
    \brief Costruisce una data di nascita da una stringa nel formato GG/MM/AAAA.
    \param dateString Data nel formato GG/MM/AAAA.
    
    \throws dataexp Se la data non è valida.
*/
dataNascita::dataNascita(const std::string& dateString)
{
    if (dateString.length() != 10 || dateString[2] != '/' || dateString[5] != '/')
        throw dataexp("Formato data non valido. Usare GG/MM/AAAA", -1);
    
    try
    {
        int giorno = std::stoi(dateString.substr(0, 2));
        int meseVal = std::stoi(dateString.substr(3, 2));
        int annoVal = std::stoi(dateString.substr(6, 4));
        
        // Validazione usando le classi base
        data tempData(giorno);
        mese tempMese(meseVal);
        anno tempAnno(annoVal);
        
        // Se arriviamo qui, è tutto valido
        *static_cast<data*>(this) = tempData;
        *static_cast<mese*>(this) = tempMese;
        *static_cast<anno*>(this) = tempAnno;
    }
    catch (const std::exception& e)
    {
        throw dataexp("Errore nel parsing della data: " + dateString, -1);
    }
}

/*!
    \fn std::string dataNascita::getDataFormatted() const
    \brief Restituisce la data di nascita formattata.
    \return Stringa nel formato DD/MM/YYYY.
*/
std::string dataNascita::getDataFormatted() const
{
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << getDate() + 1 << "/"
       << std::setw(2) << numMese() + 1 << "/"
       << std::setw(4) << (1900 + getAnno());
    return ss.str();
}

/*!
    \fn int dataNascita::getEta() const
    \brief Calcola l'età della persona in anni.
    \return Età in anni.
*/
int dataNascita::getEta() const
{
    int annoCorrente = anno::systemYear() + 1900;
    int annoBirth = getAnno() + 1900;
    int eta = annoCorrente - annoBirth;
    
    // Se il compleanno non è ancora arrivato quest'anno, sottrai 1
    int meseCorrente = mese::systemMonth();
    int meseNascita = numMese();
    
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

/*!
    \fn bool dataNascita::isValidBirthDate() const
    \brief Verifica se la data di nascita è valida.
    \return \c true se la data è una data di nascita valida, \c false altrimenti.
    
    Controlli effettuati:
    - Anno non può essere nel futuro
    - Età minima ragionevole (> 0 anni)
    - Età massima ragionevole (< 150 anni)
*/
bool dataNascita::isValidBirthDate() const
{
    int annoCorrente = anno::systemYear();
    int annoBirth = getAnno();
    
    // Anno non deve essere nel futuro
    if (annoBirth > annoCorrente)
        return false;
    
    // Età deve essere ragionevole (tra 0 e 150 anni)
    int eta = getEta();
    if (eta < 0 || eta > 150)
        return false;
    
    return true;
}

/*!
    \fn bool operator==(const dataNascita &x, const dataNascita &y)
    \relates dataNascita
    \brief Confronta due date di nascita per uguaglianza.
    \param x Prima data di nascita.
    \param y Seconda data di nascita.
    \return \c true se le date sono identiche, \c false altrimenti.
*/
bool operator==(const dataNascita &x, const dataNascita &y)
{
    return (x.getDate() == y.getDate() && 
            x.numMese() == y.numMese() && 
            x.getAnno() == y.getAnno());
}

/*!
    \fn bool operator>(const dataNascita &x, const dataNascita &y)
    \relates dataNascita
    \brief Verifica se la prima data di nascita è posteriore alla seconda.
    \param x Prima data di nascita.
    \param y Seconda data di nascita.
    \return \c true se x è più recente di y, \c false altrimenti.
*/
bool operator>(const dataNascita &x, const dataNascita &y)
{
    if (x.getAnno() != y.getAnno())
        return x.getAnno() > y.getAnno();
    if (x.numMese() != y.numMese())
        return x.numMese() > y.numMese();
    return x.getDate() > y.getDate();
}

/*!
    \fn bool operator<(const dataNascita &x, const dataNascita &y)
    \relates dataNascita
    \brief Verifica se la prima data di nascita è anteriore alla seconda.
    \param x Prima data di nascita.
    \param y Seconda data di nascita.
    \return \c true se x è più antica di y, \c false altrimenti.
*/
bool operator<(const dataNascita &x, const dataNascita &y)
{
    if (x.getAnno() != y.getAnno())
        return x.getAnno() < y.getAnno();
    if (x.numMese() != y.numMese())
        return x.numMese() < y.numMese();
    return x.getDate() < y.getDate();
}
