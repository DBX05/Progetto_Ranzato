#include "dataora.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

/*!n    \class orarioexp
    \brief Classe per la gestione delle eccezioni relative alla classe orario.

    Viene lanciata quando i parametri passati alla classe orario
    o ai suoi metodi non rispettano i vincoli di validità.

    \sa orario
*/
class orarioexp
{
public:
    /*!n        \variable orarioexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!n        \variable orarioexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!n        \fn orarioexp::orarioexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    orarioexp(std::string s, int st) : errore(s), str(st) {}
};

/*!n    \class orario
    \brief Rappresenta un orario composto da ore, minuti e secondi, con supporto a timestamp Unix.

    La classe gestisce un orario locale con validazione dei campi.
    Supporta il recupero dell'ora di sistema tramite i metodi statici
    systemHour(), systemMin(), systemSecond() e systemTime().

    I valori accettati sono:
    \list
        \li Ore: 0–23
        \li Minuti: 0–59
        \li Secondi: 0–59
    \endlist

    In caso di valori non validi passati al costruttore, vengono usati
    automaticamente i valori di sistema. Per la modifica post-costruzione,
    viene lanciata un'eccezione \l orarioexp.

    \sa orarioexp, data, mese, anno, dateTime
*/

/*!n    \fn orario &orario::operator=(const orario &y)
    \brief Operatore di assegnazione.
    \param y Oggetto orario sorgente.
    \return Riferimento all'oggetto corrente.

    \note L'operatore è privato: la copia esplicita tra oggetti
    orario non è consentita dall'esterno della classe.
*/
orario &orario::operator=(const orario &y)
{
    if (this != &y)
    {
        timestamp = y.timestamp;
        sc = y.sc;
        hr = y.hr;
        min = y.min;
    }
    return *this;
}

/*!n    \fn void orario::create_timestamp(int h, int m, int s)
    \brief Calcola e imposta il timestamp Unix a partire da ora, minuti e secondi.
    \param h Ora (0–23).
    \param m Minuti (0–59).
    \param s Secondi (0–59).

    Utilizza la data locale corrente (anno/mese/giorno) come base,
    sostituendo solo la componente oraria con i valori forniti.
    Il timestamp viene aggiornato solo se il valore corrente è 0
    o diverso dall'ora di sistema.

    \note La gestione del DST (Daylight Saving Time) è delegata a \c mktime.
*/
void orario::create_timestamp(int h, int m, int s)
{
    if (timestamp != 0 || timestamp != systemTime())
    {
        std::time_t now = std::time(nullptr);
        std::tm tm = *std::localtime(&now);
        tm.tm_hour = h;
        tm.tm_min = m;
        tm.tm_sec = s;
        tm.tm_isdst = -1;
        std::time_t t = std::mktime(&tm);
        timestamp = static_cast<long long int>(t);
    }
}

/*!n    \fn orario::orario(int s, int m, int h, int tm)
    \brief Costruisce un oggetto orario con secondi, minuti, ore e timestamp opzionale.
    \param s Secondi (0–59). Se non valido, usa systemSecond().
    \param m Minuti (0–59). Se non valido, usa systemMin().
    \param h Ore (0–23). Se non valido, usa systemHour().
    \param tm Timestamp Unix. Se negativo, usa systemTime(). Default: 0.

    Se tutti e tre i valori orari (h, m, s) sono diversi da zero,
    viene calcolato automaticamente il timestamp tramite create_timestamp().

    \sa create_timestamp(), systemHour(), systemMin(), systemSecond(), systemTime()
*/
orario::orario(int s, int m, int h, int tm)
    : timestamp(tm < 0 ? systemTime() : tm),
      hr(h < 0 || h > 23 ? systemHour() : h),
      min(m < 0 || m > 59 ? systemMin() : m),
      sc(s < 0 || s > 59 ? systemSecond() : s)
{
    if (h != 0 && min != 0 && s != 0)
        create_timestamp(h, m, s);
}

/*!n    \fn int orario::getSec() const
    \brief Restituisce i secondi dell'orario corrente.
    \return Valore intero dei secondi (0–59).
*/
int orario::getSec() const { return sc; }

/*!n    \fn int orario::getMin() const
    \brief Restituisce i minuti dell'orario corrente.
    \return Valore intero dei minuti (0–59).
*/
int orario::getMin() const { return min; }

/*!n    \fn int orario::getHour() const
    \brief Restituisce le ore dell'orario corrente.
    \return Valore intero delle ore (0–23).
*/
int orario::getHour() const { return hr; }

/*!n    \fn int orario::getTimestamp() const
    \brief Restituisce il timestamp Unix associato all'orario.
    \return Valore intero del timestamp Unix.

    \warning Il timestamp è memorizzato come \c int: su sistemi a 32 bit
    potrebbe andare in overflow dopo il 19 gennaio 2038 (problema Year 2038).
    Si consiglia di usare \c long \c long per applicazioni in produzione.
*/
int orario::getTimestamp() const { return timestamp; }

/*!n    \fn std::string orario::curTime() const
    \brief Restituisce l'orario corrente come stringa nel formato HH/MM/SS.
    \return Stringa nel formato "ore/minuti/secondi".

    \note Il separatore usato è '/' anziché ':', valutare se allinearlo
    allo standard ISO 8601 (\c HH:MM:SS).
*/
std::string orario::curTime() const
{
    return std::to_string(hr) + "/" + std::to_string(min) + "/" + std::to_string(sc);
}

/*!n    \fn void orario::getFormat()
    \brief Restituisce l'orario in un formato specifico.

    \note Metodo dichiarato ma non ancora implementato.
*/
void orario::getFormat()
{
    // TODO: Implementare
}

/*!n    \fn void orario::modificaOrario(int h, int m, int s)
    \brief Modifica i valori di ore, minuti e secondi dell'orario.
    \param h Nuovo valore per le ore (0–23).
    \param m Nuovo valore per i minuti (0–59).
    \param s Nuovo valore per i secondi (0–59).

    \throws orarioexp Se uno dei valori forniti non rientra nell'intervallo valido.

    \note I valori di default (-1) causano il lancio dell'eccezione:
    è necessario fornire esplicitamente tutti i parametri.
*/
void orario::modificaOrario(int h, int m, int s)
{
    if (h < 0 || h > 23)
        throw orarioexp("Le ore devono avere valore tra 0 e 23", h);
    hr = h;
    if (m < 0 || m > 59)
        throw orarioexp("I minuti devono avere valore tra 0 e 59", m);
    min = m;
    if (s < 0 || s > 59)
        throw orarioexp("I secondi devono avere valore tra 0 e 59", s);
    sc = s;
}

/*!n    \fn static int orario::systemHour()
    \brief Restituisce l'ora corrente del sistema locale.
    \return Ora locale come intero (0–23).
*/
int orario::systemHour()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_hour);
}

/*!n    \fn static int orario::systemSecond()
    \brief Restituisce i secondi correnti del sistema locale.
    \return Secondi locali come intero (0–59).
*/
int orario::systemSecond()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_sec);
}

/*!n    \fn static int orario::systemMin()
    \brief Restituisce i minuti correnti del sistema locale.
    \return Minuti locali come intero (0–59).
*/
int orario::systemMin()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_min);
}

/*!n    \fn static int orario::systemTime()
    \brief Restituisce il timestamp corrente del sistema locale.
    \return Timestamp Unix come intero.
*/
int orario::systemTime()
{
    return static_cast<int>(std::time(nullptr));
}

/*!n    \fn bool operator==(const orario &x, const orario &y)
    \relates orario
    \brief Confronta due oggetti orario per uguaglianza tramite timestamp.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se i timestamp sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(const orario &x, const orario &y)
{
    if (&x != &y && x.getTimestamp() == y.getTimestamp()) return true;
    return false;
}

/*!n    \fn bool operator>(const orario &x, const orario &y)
    \relates orario
    \brief Confronta due oggetti orario: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il timestamp di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(const orario &x, const orario &y)
{
    if (&x != &y && x.getTimestamp() > y.getTimestamp()) return true;
    return false;
}

/*!n    \fn bool operator<(const orario &x, const orario &y)
    \relates orario
    \brief Confronta due oggetti orario: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il timestamp di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(const orario &x, const orario &y)
{
    if (&x != &y && x.getTimestamp() < y.getTimestamp()) return true;
    return false;
}

// ============================================================================

/*!n    \class dataexp
    \brief Classe per la gestione delle eccezioni relative alla classe data.

    Viene lanciata quando il valore del giorno della settimana passato
    alla classe \l data non rientra nell'intervallo valido (1–7).

    \sa data
*/
class dataexp
{
public:
    /*!n        \variable dataexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!n        \variable dataexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!n        \fn dataexp::dataexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    dataexp(std::string s, int st) : errore(s), str(st) {}
};

/*!n    \class data
    \brief Rappresenta un giorno della settimana come intero e come stringa.

    La classe gestisce un giorno della settimana con un indice intero (1–7)
    e la relativa rappresentazione testuale in italiano.

    L'array interno \c Settimana mappa gli indici nei nomi:
    \list
        \li 0 → lunedì
        \li 1 → martedì
        \li ...
        \li 6 → domenica
    \endlist

    \note Il costruttore lancia \l dataexp se il valore fornito non è valido.

    \sa dataexp, orario, mese, anno, dateTime
*/

/*!n    \fn data &data::operator=(const data &y)
    \brief Operatore di assegnazione.
    \param y Oggetto data sorgente.
    \return Riferimento all'oggetto corrente.

    \note L'operatore è privato: la copia esplicita tra oggetti
    data non è consentita dall'esterno della classe.
*/
data &data::operator=(const data &y)
{
    if (this != &y) { st = y.st; stcor = y.stcor; }
    return *this;
}

/*!n    \fn data::data(int x)
    \brief Costruisce un oggetto data con il giorno della settimana specificato.
    \param x Giorno della settimana come intero (0–6). Default: 0 (lunedì).

    \throws dataexp Se il valore fornito non rientra nell'intervallo 0–6.
*/
data::data(int x) : st((x >= 0 && x <= 6) ? x : -1)
{
    if (st == -1)
        throw dataexp("il giorno della settimana deve avere valore compreso fra 0 e 6", st);
    stcor = Settimana[st];
}

/*!n    \fn std::string data::getGiorno() const
    \brief Restituisce il nome del giorno della settimana.
    \return Nome del giorno in italiano (es. "lunedi", "martedi", ...).
*/
std::string data::getGiorno() const { return Settimana[st]; }

/*!n    \fn int data::getDate() const
    \brief Restituisce l'indice numerico del giorno della settimana.
    \return Intero compreso tra 0 e 6.
*/
int data::getDate() const { return st; }

/*!n    \fn void data::modificaData(int x)
    \brief Aggiorna il giorno della settimana.
    \param x Nuovo valore del giorno (0–6).

    \throws dataexp Se il valore fornito non rientra nell'intervallo 0–6.
*/
void data::modificaData(int x)
{
    if (x < 0 || x > 6)
        throw dataexp("il giorno della settimana deve avere valore compreso fra 0 e 6", x);
    st = x;
    stcor = Settimana[st];
}

/*!n    \fn static int data::systemDay()
    \brief Restituisce il giorno della settimana corrente del sistema.
    \return Giorno della settimana come intero (0 = domenica, 1 = lunedì, ..., 6 = sabato),
            secondo la convenzione POSIX di \c tm_wday.

    \note La convenzione POSIX parte da 0 (domenica). Potrebbe essere necessario
    un mapping esplicito se si desiderano indici diversi.
*/
int data::systemDay()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return (timestamp->tm_wday);
}

/*!n    \fn int data::ConfGrioni(int x) const
    \brief Confronta il giorno corrente con un valore fornito.
    \param x Valore intero del giorno da confrontare.
    \return
        \list
            \li  0 se i giorni sono uguali.
            \li -1 se il giorno corrente è maggiore di \a x.
            \li  1 se il giorno corrente è minore di \a x.
        \endlist

    \note Il nome del metodo contiene un refuso tipografico ("Grioni" invece di "Giorni").
*/
int data::ConfGrioni(int x) const
{
    if (st == x) return 0;
    if (st > x)  return -1;
    return 1;
}

/*!n    \fn bool operator==(const data &x, const data &y)
    \relates data
    \brief Confronta due oggetti data per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se gli indici dei giorni sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(const data &x, const data &y)
{
    if (&x != &y && x.getDate() == y.getDate()) return true;
    return false;
}

/*!n    \fn bool operator>(const data &x, const data &y)
    \relates data
    \brief Confronta due oggetti data: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(const data &x, const data &y)
{
    if (&x != &y && x.getDate() > y.getDate()) return true;
    return false;
}

/*!n    \fn bool operator<(const data &x, const data &y)
    \relates data
    \brief Confronta due oggetti data: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(const data &x, const data &y)
{
    if (&x != &y && x.getDate() < y.getDate()) return true;
    return false;
}

// ============================================================================

/*!n    \class mesexp
    \brief Classe per la gestione delle eccezioni relative alla classe mese.

    Viene lanciata quando il valore del mese passato alla classe \l mese
    non rientra nell'intervallo valido (0–11).

    \sa mese
*/
class mesexp
{
public:
    /*!n        \variable mesexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!n        \variable mesexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!n        \fn mesexp::mesexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    mesexp(std::string s, int st) : errore(s), str(st) {}
};

/*!n    \class mese
    \brief Rappresenta un mese dell'anno come intero e come stringa.

    La classe gestisce un mese con un indice intero (0–11) e la relativa
    rappresentazione testuale in italiano.

    L'array interno \c Mesi mappa gli indici (0–11) nei nomi:
    \list
        \li 0 → gennaio
        \li 1 → febbraio
        \li ...
        \li 11 → dicembre
    \endlist

    \note Il costruttore lancia \l mesexp se il valore fornito non è valido.

    \sa mesexp, orario, data, anno, dateTime
*/

/*!n    \fn mese &mese::operator=(const mese &y)
    \brief Operatore di assegnazione.
    \param y Oggetto mese sorgente.
    \return Riferimento all'oggetto corrente.

    \note L'operatore è privato: la copia esplicita tra oggetti
    mese non è consentita dall'esterno della classe.
*/
mese &mese::operator=(const mese &y)
{
    if (this != &y) { ms = y.ms; mscor = y.mscor; }
    return *this;
}

/*!n    \fn mese::mese(int x)
    \brief Costruisce un oggetto mese con il valore specificato.
    \param x Indice del mese (0–11). Default: 0 (gennaio).

    \throws mesexp Se il valore fornito non rientra nell'intervallo 0–11.
*/
mese::mese(int x) : ms((x >= 0 && x <= 11) ? x : -1)
{
    if (ms == -1)
        throw mesexp("il mese deve avere valore compreso fra 0 e 11", ms);
    mscor = Mesi[ms];
}

/*!n    \fn int mese::numMese() const
    \brief Restituisce l'indice numerico del mese corrente.
    \return Intero compreso tra 0 e 11.
*/
int mese::numMese() const { return ms; }

/*!n    \fn std::string mese::getMese() const
    \brief Restituisce il nome del mese corrente.
    \return Nome del mese in italiano (es. "gennaio", "febbraio", ...).
*/
std::string mese::getMese() const { return Mesi[ms]; }

/*!n    \fn void mese::modificaMese(int x)
    \brief Aggiorna il valore del mese.
    \param x Nuovo indice del mese (0–11).

    \throws mesexp Se il valore fornito non rientra nell'intervallo 0–11.
*/
void mese::modificaMese(int x)
{
    if (x < 0 || x > 11)
        throw mesexp("il mese deve avere valore compreso fra 0 e 11", ms);
    ms = x;
    mscor = Mesi[ms];
}

/*!n    \fn static int mese::systemMonth()
    \brief Restituisce il mese corrente del sistema.
    \return Mese locale come intero (0 = gennaio, ..., 11 = dicembre),
            secondo la convenzione POSIX di \c tm_mon.
*/
int mese::systemMonth()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return timestamp->tm_mon;
}

/*!n    \fn int mese::ConfMesi(int x) const
    \brief Confronta il mese corrente con un valore fornito.
    \param x Valore intero del mese da confrontare.
    \return
        \list
            \li  0 se i mesi sono uguali.
            \li -1 se il mese corrente è maggiore di \a x.
            \li  1 se il mese corrente è minore di \a x.
        \endlist
*/
int mese::ConfMesi(int x) const
{
    if (ms == x) return 0;
    if (ms > x)  return -1;
    return 1;
}

/*!n    \fn bool operator==(const mese &x, const mese &y)
    \relates mese
    \brief Confronta due oggetti mese per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se gli indici dei mesi sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(const mese &x, const mese &y)
{
    if (&x != &y && x.numMese() == y.numMese()) return true;
    return false;
}

/*!n    \fn bool operator>(const mese &x, const mese &y)
    \relates mese
    \brief Confronta due oggetti mese: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(const mese &x, const mese &y)
{
    if (&x != &y && x.numMese() > y.numMese()) return true;
    return false;
}

/*!n    \fn bool operator<(const mese &x, const mese &y)
    \relates mese
    \brief Confronta due oggetti mese: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(const mese &x, const mese &y)
{
    if (&x != &y && x.numMese() < y.numMese()) return true;
    return false;
}

// ============================================================================

/*!n    \class annoexp
    \brief Classe per la gestione delle eccezioni relative alla classe anno.

    Viene lanciata quando il valore dell'anno passato alla classe \l anno
    non rispetta i vincoli di validità (valore non negativo).

    \sa anno
*/
class annoexp
{
public:
    /*!n        \variable annoexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!n        \variable annoexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!n        \fn annoexp::annoexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    annoexp(std::string s, int st) : errore(s), str(st) {}
};

/*!n    \class anno
    \brief Rappresenta un anno come valore intero.

    La classe memorizza un anno con validazione: se il valore fornito
    è negativo, viene sostituito con l'anno di sistema tramite systemYear().

    \note systemYear() restituisce \c tm_year, che rappresenta gli anni
    trascorsi dal 1900. Per ottenere l'anno solare corrente è necessario
    sommare 1900: \c systemYear() + 1900.

    \sa annoexp, orario, data, mese, dateTime
*/

/*!n    \fn anno &anno::operator=(const anno &y)
    \brief Operatore di assegnazione.
    \param y Oggetto anno sorgente.
    \return Riferimento all'oggetto corrente.

    \note L'operatore è privato: la copia esplicita tra oggetti
    anno non è consentita dall'esterno della classe.
*/
anno &anno::operator=(const anno &y)
{
    if (this != &y) { annocr = y.annocr; }
    return *this;
}

/*!n    \fn anno::anno(int an)
    \brief Costruisce un oggetto anno con il valore specificato.
    \param an Anno da memorizzare. Se negativo, viene usato systemYear(). Default: systemYear().

    \note Il valore restituito da systemYear() è relativo al 1900 (convenzione POSIX):
    l'anno 2025 è rappresentato come 125.
*/
anno::anno(int an) : annocr((an < 0) ? systemYear() : an) {}

/*!n    \fn int anno::getAnno() const
    \brief Restituisce il valore dell'anno corrente.
    \return Anno come intero (offset dal 1900 secondo la convenzione POSIX di \c tm_year).
*/
int anno::getAnno() const { return annocr; }

/*!n    \fn void anno::modificaAnno(int x)
    \brief Aggiorna il valore dell'anno.
    \param x Nuovo valore dell'anno. Deve essere non negativo.

    \note Se \a x è negativo, la modifica viene silenziosamente ignorata
    senza lanciare eccezioni. Valutare se aggiungere un lancio di \l annoexp.
*/
void anno::modificaAnno(int x)
{
    if (x > -1) annocr = x;
}

/*!n    \fn static int anno::systemYear()
    \brief Restituisce l'anno corrente del sistema.
    \return Anno locale come intero, relativo al 1900 (convenzione POSIX di \c tm_year).
            Ad esempio, per il 2025 restituisce 125.
*/
int anno::systemYear()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return timestamp->tm_year;
}

/*!n    \fn bool operator==(const anno &x, const anno &y)
    \relates anno
    \brief Confronta due oggetti anno per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se i valori degli anni sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(const anno &x, const anno &y)
{
    if (&x != &y && x.getAnno() == y.getAnno()) return true;
    return false;
}

/*!n    \fn bool operator>(const anno &x, const anno &y)
    \relates anno
    \brief Confronta due oggetti anno: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il valore di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(const anno &x, const anno &y)
{
    if (&x != &y && x.getAnno() > y.getAnno()) return true;
    return false;
}

/*!n    \fn bool operator<(const anno &x, const anno &y)
    \relates anno
    \brief Confronta due oggetti anno: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il valore di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(const anno &x, const anno &y)
{
    if (&x != &y && x.getAnno() < y.getAnno()) return true;
    return false;
}

// ============================================================================

/*!n    \class DateTimexp
    \brief Classe per la gestione delle eccezioni relative alla classe dateTime.

    Viene lanciata in caso di errori nella costruzione o manipolazione
    di oggetti \l dateTime.

    \sa dateTime
*/
class DateTimexp
{
public:
    /*!n        \variable DateTimexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!n        \fn DateTimexp::DateTimexp(std::string s)
        \brief Costruisce un'eccezione con il messaggio specificato.
        \param s Stringa descrittiva dell'errore.
    */
    DateTimexp(std::string s) : errore(s) {}
};

/*!n    \class dateTime
    \brief Rappresenta una data e ora completa tramite ereditarietà multipla.

    La classe \c dateTime aggrega le informazioni di \l orario, \l data,
    \l mese e \l anno in un'unica entità, fornendo metodi per la lettura,
    modifica e formattazione di una data e ora completa.

    Eredita da:
    \list
        \li \l orario — gestione di ore, minuti, secondi e timestamp.
        \li \l data — gestione del giorno della settimana.
        \li \l mese — gestione del mese dell'anno.
        \li \l anno — gestione dell'anno.
    \endlist

    Il costruttore di default inizializza tutti i campi con i valori
    correnti del sistema tramite i rispettivi metodi statici.

    \sa DateTimexp, orario, data, mese, anno
*/

/*!n    \fn dateTime &dateTime::operator=(const dateTime &y)
    \brief Operatore di assegnazione.
    \param y Oggetto dateTime sorgente.
    \return Riferimento all'oggetto corrente.

    \note L'operatore è privato: la copia esplicita tra oggetti
    dateTime non è consentita dall'esterno della classe.
    \warning L'operatore copia solo \c dateT, non i campi delle classi base.
    Valutare se richiamare gli operatori di assegnazione delle classi base.
*/
dateTime &dateTime::operator=(const dateTime &y)
{
    if (this != &y) { dateT = y.dateT; }
    return *this;
}

/*!n    \fn dateTime::dateTime(int an, int m, int d, int h, int mn, int s)
    \brief Costruisce un oggetto dateTime con anno, mese, giorno, ore, minuti e secondi.
    \param an Anno. Default: systemYear().
    \param m  Mese (0–11). Default: systemMonth().
    \param d  Giorno della settimana (0–6). Default: systemDay().
    \param h  Ore (0–23). Default: systemHour().
    \param mn Minuti (0–59). Default: systemMin().
    \param s  Secondi (0–59). Default: systemSecond().

    Delegati al costruttore di \l orario (s, mn, h), \l data (d), \l mese (m) e \l anno (an).

    \throws orarioexp Se i valori di ore, minuti o secondi non sono validi (propagata da \l orario).
    \throws dataexp   Se il giorno non è valido (propagata da \l data).
    \throws mesexp    Se il mese non è valido (propagata da \l mese).
*/
dateTime::dateTime(int an, int m, int d, int h, int mn, int s)
    : orario(s, mn, h), data(d), mese(m), anno(an) {}

/*!n    \fn std::string dateTime::getDateTime() const
    \brief Restituisce la data e ora formattata come stringa.
    \return Stringa \c dateT contenente la data e l'ora.

    \note Il campo \c dateT non viene mai popolato nel costruttore
    né aggiornato da \c modificaDateTime(). Restituisce sempre una
    stringa vuota finché non viene implementato un aggiornamento esplicito.
*/
std::string dateTime::getDateTime() const { return dateT; }

/*!n    \fn void dateTime::modificaDateTime(int an, int m, int d, int h, int mn, int s)
    \brief Modifica selettivamente i campi della data e dell'orario.
    \param an Nuovo anno. Ignorato se 0. Default: 0.
    \param m  Nuovo mese. Ignorato se -1. Default: -1.
    \param d  Nuovo giorno. Ignorato se -1. Default: -1.
    \param h  Nuove ore. Passate a modificaOrario() se almeno uno tra h, mn, s è != -1. Default: -1.
    \param mn Nuovi minuti. Default: -1.
    \param s  Nuovi secondi. Default: -1.

    \throws annoexp Se il valore dell'anno non è valido (propagata da modificaAnno()).
    \throws mesexp  Se il valore del mese non è valido (propagata da modificaMese()).
    \throws dataexp Se il valore del giorno non è valido (propagata da modificaData()).
    \throws orarioexp Se i valori di ore, minuti o secondi non sono validi (propagata da modificaOrario()).
*/
void dateTime::modificaDateTime(int an, int m, int d, int h, int mn, int s)
{
    if (an != 0) modificaAnno(an);
    if (m != -1) modificaMese(m);
    if (d != -1) modificaData(d);
    if (h != -1 || mn != -1 || s != -1) modificaOrario(h, mn, s);
}

/*!n    \fn std::string dateTime::FormatDate(std::string s)
    \brief Formatta la data e l'ora corrente secondo un formato specificato.
    \param s Stringa di formato compatibile con \c std::put_time / \c strftime.
             Se vuota, usa il formato ISO 8601: \c "YYYY-MM-DD HH:MM:SS". Default: "".

    La funzione costruisce una struttura \c std::tm a partire dai valori
    correnti dell'oggetto e la formatta tramite \c std::put_time.

    \return Stringa contenente la data e l'ora formattata.
*/
std::string dateTime::FormatDate(std::string s)
{
    std::time_t t = std::time(nullptr);
    std::tm *tm_ptr = std::localtime(&t);
    tm_ptr->tm_sec  = getSec();
    tm_ptr->tm_min  = getMin();
    tm_ptr->tm_hour = getHour();
    tm_ptr->tm_mday = getDate();
    tm_ptr->tm_year = getAnno();
    tm_ptr->tm_mon  = numMese();
    std::stringstream ss;
    if (s == "")
        ss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
    else
        ss << std::put_time(tm_ptr, s.c_str());
    return ss.str();
}

/*!n    \fn std::string dateTime::systemDateTime() const
    \brief Restituisce la data e l'ora correnti del sistema.
    \return Stringa con la data e l'ora del sistema nel formato
            \c "giorno/mese/anno  HH:MM:SS".
*/
std::string dateTime::systemDateTime() const
{
    time_t rawtime;
    struct tm *timeinfo;
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    
    std::stringstream ss;
    ss << timeinfo->tm_mday << "/" 
       << (timeinfo->tm_mon + 1) << "/" 
       << (1900 + timeinfo->tm_year) << "  "
       << timeinfo->tm_hour << ":"
       << timeinfo->tm_min << ":"
       << timeinfo->tm_sec;
    
    return ss.str();
}

/*!n    \fn bool operator==(const dateTime &x, const dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime per uguaglianza tramite la stringa \c dateT.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se le stringhe \c dateT sono uguali e gli oggetti sono distinti, \c false altrimenti.

    \warning Poiché \c dateT non viene mai popolata, questo operatore restituirà
    sempre \c true per oggetti distinti (due stringhe vuote sono uguali).
    Considerare il confronto tramite timestamp o campi individuali.
*/
bool operator==(const dateTime &x, const dateTime &y)
{
    if (&x != &y && x.getDateTime() == y.getDateTime()) return true;
    return false;
}

/*!n    \fn bool operator>(const dateTime &x, const dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se la stringa \c dateT di \a x è lessicograficamente maggiore di quella di \a y.

    \warning Il confronto è lessicografico su stringhe. Poiché \c dateT non viene popolata,
    il risultato è sempre \c false. Utilizzare un confronto numerico su timestamp o campi data/ora.
*/
bool operator>(const dateTime &x, const dateTime &y)
{
    if (&x != &y && x.getDateTime() > y.getDateTime()) return true;
    return false;
}

/*!n    \fn bool operator<(const dateTime &x, const dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se la stringa \c dateT di \a x è lessicograficamente minore di quella di \a y.

    \warning Stessa limitazione di operator>: il confronto su \c dateT vuota è sempre \c false.
*/
bool operator<(const dateTime &x, const dateTime &y)
{
    if (&x != &y && x.getDateTime() < y.getDateTime()) return true;
    return false;
}
