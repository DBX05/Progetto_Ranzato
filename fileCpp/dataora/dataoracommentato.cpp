#include <string>
#include <ctime>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

/*!
    \class orarioexp
    \brief Classe per la gestione delle eccezioni relative alla classe orario.

    Viene lanciata quando i parametri passati alla classe orario
    o ai suoi metodi non rispettano i vincoli di validità.

    \sa orario
*/
class orarioexp
{
public:
    /*!
        \variable orarioexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!
        \variable orarioexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!
        \fn orarioexp::orarioexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    orarioexp(std::string s, int st) : errore(s), str(st) {};
};

/*!
    \class orario
    \brief Rappresenta un orario composto da ore, minuti e secondi, con supporto a timestamp Unix.

    La classe gestisce un orario locale con validazione dei campi.
    Supporta il recupero dell'ora di sistema tramite i metodi statici
    systemHour(), systemMin(), systemSecond() e systemTime().

    I valori accettati sono:
    \list
        \li Ore: 0–23
        \li Minuti: 0–60
        \li Secondi: 0–60
    \endlist

    In caso di valori non validi passati al costruttore, vengono usati
    automaticamente i valori di sistema. Per la modifica post-costruzione,
    viene lanciata un'eccezione \l orarioexp.

    \sa orarioexp, data, mese, anno, dateTime
*/
class orario
{
private:
    int timestamp;
    int sc, min, hr;

    /*!
        \fn orario &orario::operator=(orario &y)
        \brief Operatore di assegnazione.
        \param y Oggetto orario sorgente.
        \return Riferimento all'oggetto corrente.

        \note L'operatore è privato: la copia esplicita tra oggetti
        orario non è consentita dall'esterno della classe.
    */
    orario &operator=(orario &y)
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

    /*!
        \fn void orario::create_timestamp(int h, int m, int s)
        \brief Calcola e imposta il timestamp Unix a partire da ora, minuti e secondi.
        \param h Ora (0–23).
        \param m Minuti (0–60).
        \param s Secondi (0–60).

        Utilizza la data locale corrente (anno/mese/giorno) come base,
        sostituendo solo la componente oraria con i valori forniti.
        Il timestamp viene aggiornato solo se il valore corrente è 0
        o diverso dall'ora di sistema.

        \note La gestione del DST (Daylight Saving Time) è delegata a \c mktime.
    */
    void create_timestamp(int h, int m, int s)
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

public:
    /*!
        \fn orario::orario(int s, int m, int h, int tm)
        \brief Costruisce un oggetto orario con secondi, minuti, ore e timestamp opzionale.
        \param s Secondi (0–60). Se non valido, usa systemSecond().
        \param m Minuti (0–60). Se non valido, usa systemMin().
        \param h Ore (0–23). Se non valido, usa systemHour().
        \param tm Timestamp Unix. Se negativo, usa systemTime(). Default: 0.

        Se tutti e tre i valori orari (h, m, s) sono diversi da zero,
        viene calcolato automaticamente il timestamp tramite create_timestamp().

        \sa create_timestamp(), systemHour(), systemMin(), systemSecond(), systemTime()
    */
    orario(int s = 0, int m = 0, int h = 0, int tm = 0)
        : timestamp(tm < 0 ? systemTime() : tm),
          hr(h < 0 || h > 23 ? systemHour() : h),
          min(m < 0 || m > 60 ? systemMin() : m),
          sc(s < 0 || s > 60 ? systemSecond() : s)
    {
        if (h != 0 && min != 0 && s != 0)
            create_timestamp(h, m, s);
    }

    /*!
        \fn int orario::getSec() const
        \brief Restituisce i secondi dell'orario corrente.
        \return Valore intero dei secondi (0–60).
    */
    int getSec() const { return sc; }

    /*!
        \fn int orario::getMin() const
        \brief Restituisce i minuti dell'orario corrente.
        \return Valore intero dei minuti (0–60).
    */
    int getMin() const { return min; }

    /*!
        \fn int orario::getHour() const
        \brief Restituisce le ore dell'orario corrente.
        \return Valore intero delle ore (0–23).
    */
    int getHour() const { return hr; }

    /*!
        \fn int orario::getTimestamp() const
        \brief Restituisce il timestamp Unix associato all'orario.
        \return Valore intero del timestamp Unix.

        \warning Il timestamp è memorizzato come \c int: su sistemi a 32 bit
        potrebbe andare in overflow dopo il 19 gennaio 2038 (problema Year 2038).
        Si consiglia di usare \c long \c long per applicazioni in produzione.
    */
    int getTimestamp() const { return timestamp; }

    /*!
        \fn std::string orario::curTime() const
        \brief Restituisce l'orario corrente come stringa nel formato HH/MM/SS.
        \return Stringa nel formato "ore/minuti/secondi".

        \note Il separatore usato è '/' anziché ':', valutare se allinearlo
        allo standard ISO 8601 (\c HH:MM:SS).
    */
    std::string curTime() const
    {
        return std::to_string(hr) + "/" + std::to_string(min) + "/" + std::to_string(sc);
    }

    /*!
        \fn void orario::getFormat()
        \brief Restituisce l'orario in un formato specifico.

        \note Metodo dichiarato ma non ancora implementato.
    */
    void getFormat();

    /*!
        \fn void orario::modificaOrario(int h, int m, int s)
        \brief Modifica i valori di ore, minuti e secondi dell'orario.
        \param h Nuovo valore per le ore (0–23).
        \param m Nuovo valore per i minuti (0–60).
        \param s Nuovo valore per i secondi (0–60).

        \throws orarioexp Se uno dei valori forniti non rientra nell'intervallo valido.

        \note I valori di default (-1) causano sempre il lancio dell'eccezione:
        è necessario fornire esplicitamente tutti i parametri.
    */
    void modificaOrario(int h = -1, int m = -1, int s = -1)
    {
        if (h < 0 || h > 23)
            throw orarioexp("Le ore devono avere valore fra le 1 e 23", h);
        hr = h;
        if (m < 0 || m > 60)
            throw orarioexp("I minuti devono avere valore fra le 1 e 60", m);
        min = m;
        if (s < 0 || s > 60)
            throw orarioexp("I secondi devono avere valore fra le 1 e 60", s);
        sc = s;
    }

    /*!
        \fn static int orario::systemHour()
        \brief Restituisce l'ora corrente del sistema locale.
        \return Ora locale come intero (0–23).
    */
    static int systemHour()
    {
        std::time_t now = std::time(nullptr);
        std::tm *localTime = std::localtime(&now);
        return int(localTime->tm_hour);
    }

    /*!
        \fn static int orario::systemSecond()
        \brief Restituisce i secondi correnti del sistema locale.
        \return Secondi locali come intero (0–60).
    */
    static int systemSecond()
    {
        std::time_t now = std::time(nullptr);
        std::tm *localTime = std::localtime(&now);
        return int(localTime->tm_sec);
    }

    /*!
        \fn static int orario::systemMin()
        \brief Restituisce i minuti correnti del sistema locale.
        \return Minuti locali come intero (0–60).
    */
    static int systemMin()
    {
        std::time_t now = std::time(nullptr);
        std::tm *localTime = std::localtime(&now);
        return int(localTime->tm_min);
    }

    /*!
        \fn static int orario::systemTime()
        \brief Restituisce il timestamp corrente del sistema locale.
        \return Timestamp Unix come intero.

        \warning L'implementazione corrente esegue un cast di un puntatore
        \c tm* a \c int, il che produce un comportamento indefinito.
        Sostituire con \c static_cast<int>(std::time(nullptr)).
    */
    static int systemTime()
    {
        std::time_t now = std::time(nullptr);
        std::tm *localTime = std::localtime(&now);
        return int(localTime);
    }
};

/*!
    \fn bool operator==(orario &x, orario &y)
    \relates orario
    \brief Confronta due oggetti orario per uguaglianza tramite timestamp.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se i timestamp sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() == y.getTimestamp()) return true;
    return false;
}

/*!
    \fn bool operator>(orario &x, orario &y)
    \relates orario
    \brief Confronta due oggetti orario: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il timestamp di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() > y.getTimestamp()) return true;
    return false;
}

/*!
    \fn bool operator<(orario &x, orario &y)
    \relates orario
    \brief Confronta due oggetti orario: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il timestamp di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() < y.getTimestamp()) return true;
    return false;
}

/*!
    \class dataexp
    \brief Classe per la gestione delle eccezioni relative alla classe data.

    Viene lanciata quando il valore del giorno della settimana passato
    alla classe \l data non rientra nell'intervallo valido (1–7).

    \sa data
*/
class dataexp
{
public:
    /*!
        \variable dataexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!
        \variable dataexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!
        \fn dataexp::dataexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    dataexp(std::string s, int st) : errore(s), str(st) {};
};

/*!
    \class data
    \brief Rappresenta un giorno della settimana come intero e come stringa.

    La classe gestisce un giorno della settimana con un indice intero (1–7)
    e la relativa rappresentazione testuale in italiano.

    L'array interno \c Settimana mappa gli indici nei nomi:
    \list
        \li 1 → lunedì
        \li 2 → martedì
        \li ...
        \li 7 → domenica
    \endlist

    \note Il costruttore lancia \l dataexp se il valore fornito non è valido.

    \sa dataexp, orario, mese, anno, dateTime
*/
class data
{
private:
    std::string Settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
    int st;
    std::string stcor;

    /*!
        \fn data &data::operator=(data &y)
        \brief Operatore di assegnazione.
        \param y Oggetto data sorgente.
        \return Riferimento all'oggetto corrente.

        \note L'operatore è privato: la copia esplicita tra oggetti
        data non è consentita dall'esterno della classe.
    */
    data &operator=(data &y)
    {
        if (this != &y) { st = y.st; stcor = y.stcor; }
        return *this;
    }

public:
    /*!
        \fn data::data(int x)
        \brief Costruisce un oggetto data con il giorno della settimana specificato.
        \param x Giorno della settimana come intero (1–7). Default: 0.

        \throws dataexp Se il valore fornito non rientra nell'intervallo 1–7.

        \warning La condizione di validità nel costruttore usa \c || anziché \c &&:
        valori fuori range come 14 non vengono rigettati correttamente.
        Si consiglia di correggere in \c (x >= 1 && x <= 7).
    */
    data(int x = 0) : st((x - 1 > -1 || x - 1 < 8) ? x : -1)
    {
        if (st == -1)
            throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", st);
        stcor = Settimana[st];
    }

    /*!
        \fn std::string data::getGiorno() const
        \brief Restituisce il nome del giorno della settimana.
        \return Nome del giorno in italiano (es. "lunedi", "martedi", ...).
    */
    std::string getGiorno() const { return Settimana[st]; }

    /*!
        \fn int data::getDate() const
        \brief Restituisce l'indice numerico del giorno della settimana.
        \return Intero compreso tra 1 e 7.
    */
    int getDate() const { return st; }

    /*!
        \fn void data::modificaData(int x)
        \brief Aggiorna il giorno della settimana.
        \param x Nuovo valore del giorno (1–7).

        \throws dataexp Se il valore fornito non rientra nell'intervallo 0–7.

        \note Il controllo usa \c x < 0 || x > 7: il valore 0 è accettato
        ma non corrisponde a nessun giorno valido nell'array \c Settimana.
        Valutare se restringere il range a 1–7.
    */
    void modificaData(int x)
    {
        if (x < 0 || x > 7)
            throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", x);
        st = x;
        stcor = Settimana[st];
    }

    /*!
        \fn static int data::systemDay()
        \brief Restituisce il giorno della settimana corrente del sistema.
        \return Giorno della settimana come intero (0 = domenica, 1 = lunedì, ..., 6 = sabato),
                secondo la convenzione POSIX di \c tm_wday.

        \note La convenzione POSIX parte da 0 (domenica), mentre l'array \c Settimana
        parte da lunedì (indice 0). Potrebbe essere necessario un mapping esplicito.
    */
    static int systemDay()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return (timestamp->tm_wday);
    }

    /*!
        \fn int data::ConfGrioni(int x) const
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
    int ConfGrioni(int x) const
    {
        if (st == x) return 0;
        if (st > x)  return -1;
        return 1;
    }
};

/*!
    \fn bool operator==(data &x, data &y)
    \relates data
    \brief Confronta due oggetti data per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se gli indici dei giorni sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(data &x, data &y)
{
    if (&x != &y && x.getDate() == y.getDate()) return true;
    return false;
}

/*!
    \fn bool operator>(data &x, data &y)
    \relates data
    \brief Confronta due oggetti data: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(data &x, data &y)
{
    if (&x != &y && x.getDate() > y.getDate()) return true;
    return false;
}

/*!
    \fn bool operator<(data &x, data &y)
    \relates data
    \brief Confronta due oggetti data: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(data &x, data &y)
{
    if (&x != &y && x.getDate() < y.getDate()) return true;
    return false;
}

/*!
    \class mesexp
    \brief Classe per la gestione delle eccezioni relative alla classe mese.

    Viene lanciata quando il valore del mese passato alla classe \l mese
    non rientra nell'intervallo valido (1–12).

    \sa mese
*/
class mesexp
{
public:
    /*!
        \variable mesexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!
        \variable mesexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!
        \fn mesexp::mesexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    mesexp(std::string s, int st) : errore(s), str(st) {};
};

/*!
    \class mese
    \brief Rappresenta un mese dell'anno come intero e come stringa.

    La classe gestisce un mese con un indice intero e la relativa
    rappresentazione testuale in italiano.

    L'array interno \c Mesi mappa gli indici (0–11) nei nomi:
    \list
        \li 0 → gennaio
        \li 1 → febbraio
        \li ...
        \li 11 → dicembre
    \endlist

    \note Il costruttore lancia \l mesexp se il valore fornito non è valido.

    \warning La condizione di validità nel costruttore usa \c || anziché \c &&,
    rendendo il controllo sempre vero. Valori fuori range come 14 non vengono
    rigettati. Correggere in \c (x >= 1 && x <= 12).

    \sa mesexp, orario, data, anno, dateTime
*/
class mese
{
private:
    std::string Mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno",
                             "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
    int ms;
    std::string mscor;

    /*!
        \fn mese &mese::operator=(mese &y)
        \brief Operatore di assegnazione.
        \param y Oggetto mese sorgente.
        \return Riferimento all'oggetto corrente.

        \note L'operatore è privato: la copia esplicita tra oggetti
        mese non è consentita dall'esterno della classe.
    */
    mese &operator=(mese &y)
    {
        if (this != &y) { ms = y.ms; mscor = y.mscor; }
        return *this;
    }

public:
    /*!
        \fn mese::mese(int x)
        \brief Costruisce un oggetto mese con il valore specificato.
        \param x Indice del mese (1–12). Default: 0.

        \throws mesexp Se il valore calcolato risulta -1 (non valido).

        \warning La condizione di validità \c (x - 1 < 12 || x - 1 > -1)
        è sempre vera per qualsiasi valore intero: il controllo non rigetta
        mai valori fuori range. Correggere in \c (x >= 1 && x <= 12).
    */
    mese(int x = 0) : ms((x - 1 < 12 || x - 1 > -1) ? x : -1)
    {
        if (ms == -1)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12", ms);
        mscor = Mesi[ms];
    }

    /*!
        \fn int mese::numMese()
        \brief Restituisce l'indice numerico del mese corrente.
        \return Intero compreso tra 0 e 11.

        \note Il metodo non è dichiarato \c const, pur non modificando lo stato
        dell'oggetto. Si consiglia di aggiungere il qualificatore \c const.
    */
    int numMese() { return ms; }

    /*!
        \fn std::string mese::getMese() const
        \brief Restituisce il nome del mese corrente.
        \return Nome del mese in italiano (es. "gennaio", "febbraio", ...).
    */
    std::string getMese() const { return Mesi[ms]; }

    /*!
        \fn void mese::modificaMese(int x)
        \brief Aggiorna il valore del mese.
        \param x Nuovo indice del mese (0–11).

        \throws mesexp Se il valore fornito non rientra nell'intervallo 0–11.
    */
    void modificaMese(int x)
    {
        if (x < 0 || x > 11)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12", ms);
        ms = x;
        mscor = Mesi[ms];
    }

    /*!
        \fn static int mese::systemMonth()
        \brief Restituisce il mese corrente del sistema.
        \return Mese locale come intero (0 = gennaio, ..., 11 = dicembre),
                secondo la convenzione POSIX di \c tm_mon.
    */
    static int systemMonth()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return timestamp->tm_mon;
    }

    /*!
        \fn int mese::ConfMesi(int x) const
        \brief Confronta il mese corrente con un valore fornito.
        \param x Valore intero del mese da confrontare.
        \return
            \list
                \li  0 se i mesi sono uguali.
                \li -1 se il mese corrente è maggiore di \a x.
                \li  1 se il mese corrente è minore di \a x.
            \endlist
    */
    int ConfMesi(int x) const
    {
        if (ms == x) return 0;
        if (ms > x)  return -1;
        return 1;
    }
};

/*!
    \fn bool operator==(mese &x, mese &y)
    \relates mese
    \brief Confronta due oggetti mese per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se gli indici dei mesi sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(mese &x, mese &y)
{
    if (&x != &y && x.numMese() == y.numMese()) return true;
    return false;
}

/*!
    \fn bool operator>(mese &x, mese &y)
    \relates mese
    \brief Confronta due oggetti mese: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(mese &x, mese &y)
{
    if (&x != &y && x.numMese() > y.numMese()) return true;
    return false;
}

/*!
    \fn bool operator<(mese &x, mese &y)
    \relates mese
    \brief Confronta due oggetti mese: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se l'indice di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(mese &x, mese &y)
{
    if (&x != &y && x.numMese() < y.numMese()) return true;
    return false;
}

/*!
    \class annoexp
    \brief Classe per la gestione delle eccezioni relative alla classe anno.

    Viene lanciata quando il valore dell'anno passato alla classe \l anno
    non rispetta i vincoli di validità (valore non negativo).

    \sa anno
*/
class annoexp
{
public:
    /*!
        \variable annoexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!
        \variable annoexp::str
        \brief Valore intero non valido che ha causato l'eccezione.
    */
    int str;

    /*!
        \fn annoexp::annoexp(std::string s, int st)
        \brief Costruisce un'eccezione con messaggio e valore non valido.
        \param s Stringa descrittiva dell'errore.
        \param st Valore intero che ha causato l'eccezione.
    */
    annoexp(std::string s, int st) : errore(s), str(st) {};
};

/*!
    \class anno
    \brief Rappresenta un anno come valore intero.

    La classe memorizza un anno con validazione: se il valore fornito
    è negativo, viene sostituito con l'anno di sistema tramite systemYear().

    \note systemYear() restituisce \c tm_year, che rappresenta gli anni
    trascorsi dal 1900. Per ottenere l'anno solare corrente è necessario
    sommare 1900: \c systemYear() + 1900.

    \sa annoexp, orario, data, mese, dateTime
*/
class anno
{
private:
    int annocr;

    /*!
        \fn anno &anno::operator=(anno &y)
        \brief Operatore di assegnazione.
        \param y Oggetto anno sorgente.
        \return Riferimento all'oggetto corrente.

        \note L'operatore è privato: la copia esplicita tra oggetti
        anno non è consentita dall'esterno della classe.
    */
    anno &operator=(anno &y)
    {
        if (this != &y) { annocr = y.annocr; }
        return *this;
    }

public:
    /*!
        \fn anno::anno(int an)
        \brief Costruisce un oggetto anno con il valore specificato.
        \param an Anno da memorizzare. Se negativo, viene usato systemYear(). Default: systemYear().

        \note Il valore restituito da systemYear() è relativo al 1900 (convenzione POSIX):
        l'anno 2025 è rappresentato come 125.
    */
    anno(int an = systemYear()) : annocr((an < 0) ? systemYear() : an) {}

    /*!
        \fn int anno::getAnno() const
        \brief Restituisce il valore dell'anno corrente.
        \return Anno come intero (offset dal 1900 secondo la convenzione POSIX di \c tm_year).
    */
    int getAnno() const { return annocr; }

    /*!
        \fn void anno::modificaAnno(int x)
        \brief Aggiorna il valore dell'anno.
        \param x Nuovo valore dell'anno. Deve essere non negativo.

        \note Se \a x è negativo, la modifica viene silenziosamente ignorata
        senza lanciare eccezioni. Valutare se aggiungere un lancio di \l annoexp.
    */
    void modificaAnno(int x)
    {
        if (x > -1) annocr = x;
    }

    /*!
        \fn static int anno::systemYear()
        \brief Restituisce l'anno corrente del sistema.
        \return Anno locale come intero, relativo al 1900 (convenzione POSIX di \c tm_year).
                Ad esempio, per il 2025 restituisce 125.
    */
    static int systemYear()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return timestamp->tm_year;
    }
};

/*!
    \fn bool operator==(anno &x, anno &y)
    \relates anno
    \brief Confronta due oggetti anno per uguaglianza.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se i valori degli anni sono uguali e gli oggetti sono distinti, \c false altrimenti.
*/
bool operator==(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() == y.getAnno()) return true;
    return false;
}

/*!
    \fn bool operator>(anno &x, anno &y)
    \relates anno
    \brief Confronta due oggetti anno: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il valore di \a x è maggiore di quello di \a y, \c false altrimenti.
*/
bool operator>(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() > y.getAnno()) return true;
    return false;
}

/*!
    \fn bool operator<(anno &x, anno &y)
    \relates anno
    \brief Confronta due oggetti anno: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se il valore di \a x è minore di quello di \a y, \c false altrimenti.
*/
bool operator<(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() < y.getAnno()) return true;
    return false;
}

/*!
    \class DateTimexp
    \brief Classe per la gestione delle eccezioni relative alla classe dateTime.

    Viene lanciata in caso di errori nella costruzione o manipolazione
    di oggetti \l dateTime.

    \sa dateTime
*/
class DateTimexp
{
public:
    /*!
        \variable DateTimexp::errore
        \brief Messaggio descrittivo dell'errore verificatosi.
    */
    std::string errore;

    /*!
        \fn DateTimexp::DateTimexp(std::string s)
        \brief Costruisce un'eccezione con il messaggio specificato.
        \param s Stringa descrittiva dell'errore.
    */
    DateTimexp(std::string s) : errore(s) {}
};

/*!
    \class dateTime
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
class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;

    /*!
        \fn dateTime &dateTime::operator=(dateTime &y)
        \brief Operatore di assegnazione.
        \param y Oggetto dateTime sorgente.
        \return Riferimento all'oggetto corrente.

        \note L'operatore è privato: la copia esplicita tra oggetti
        dateTime non è consentita dall'esterno della classe.
        \warning L'operatore copia solo \c dateT, non i campi delle classi base.
        Valutare se richiamare gli operatori di assegnazione delle classi base.
    */
    dateTime &operator=(dateTime &y)
    {
        if (this != &y) { dateT = y.dateT; }
        return *this;
    }

public:
    /*!
        \fn dateTime::dateTime(int an, int m, int d, int h, int mn, int s)
        \brief Costruisce un oggetto dateTime con anno, mese, giorno, ore, minuti e secondi.
        \param an Anno. Default: systemYear().
        \param m  Mese (0–11). Default: systemMonth().
        \param d  Giorno della settimana (1–7). Default: systemDay().
        \param h  Ore (0–23). Default: systemHour().
        \param mn Minuti (0–60). Default: systemMin().
        \param s  Secondi (0–60). Default: systemSecond().

        Delegati al costruttore di \l orario (s, mn, h), \l data (d), \l mese (m) e \l anno (an).

        \throws orarioexp Se i valori di ore, minuti o secondi non sono validi (propagata da \l orario).
        \throws dataexp   Se il giorno non è valido (propagata da \l data).
        \throws mesexp    Se il mese non è valido (propagata da \l mese).
    */
    dateTime(int an = systemYear(), int m = systemMonth(), int d = systemDay(),
             int h = systemHour(), int mn = systemMin(), int s = systemSecond())
        : orario(s, mn, h), data(d), mese(m), anno(an) {}

    /*!
        \fn std::string dateTime::getDateTime() const
        \brief Restituisce la data e ora formattata come stringa.
        \return Stringa \c dateT contenente la data e l'ora.

        \note Il campo \c dateT non viene mai popolato nel costruttore
        né aggiornato da \c modificaDateTime(). Restituisce sempre una
        stringa vuota finché non viene implementato un aggiornamento esplicito.
    */
    std::string getDateTime() const { return dateT; }

    /*!
        \fn void dateTime::modificaDateTime(int an, int m, int d, int h, int mn, int s)
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
    void modificaDateTime(int an = 0, int m = -1, int d = -1, int h = -1, int mn = -1, int s = -1)
    {
        if (an != 0) modificaAnno(an);
        if (m != -1) modificaMese(m);
        if (d != -1) modificaData(d);
        if (h != -1 || mn != -1 || s != -1) modificaOrario(h, mn, s);
    }

    /*!
        \fn void dateTime::FormatDate(std::string s)
        \brief Formatta la data e l'ora corrente secondo un formato specificato.
        \param s Stringa di formato compatibile con \c std::put_time / \c strftime.
                 Se vuota, usa il formato ISO 8601: \c "YYYY-MM-DD HH:MM:SS". Default: "".

        La funzione costruisce una struttura \c std::tm a partire dai valori
        correnti dell'oggetto e la formatta tramite \c std::put_time.

        \warning Il risultato della formattazione viene scritto su uno \c std::stringstream
        locale ma non viene né restituito né assegnato ad alcun membro dell'oggetto.
        Occorre restituire o memorizzare \c ss.str() per rendere il metodo utile.

        \note Il parametro \a s, se non vuoto, viene passato come \c const char*
        a \c std::put_time tramite \c &s, il che è errato: occorre usare \c s.c_str().
    */
    void FormatDate(std::string s = "")
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
            ss << std::put_time(tm_ptr, &s);
    }

    /*!
        \fn std::string dateTime::systemDateTime() const
        \brief Stampa e restituisce la data e l'ora correnti del sistema.
        \return Stringa con la data e l'ora del sistema nel formato
                \c "giorno/mese/anno  HH:MM:SS".

        \warning Il metodo usa \c printf per stampare la data su stdout, ma
        non restituisce esplicitamente alcun valore (\c return mancante):
        il comportamento è undefined. Sostituire con una costruzione tramite
        \c std::string e \c return esplicito.

        \note L'anno viene corretto sommando 1900 a \c tm_year (comportamento corretto).
              Il mese non viene corretto: \c tm_mon è 0-based (0 = gennaio), quindi
              per la visualizzazione occorre sommare 1.
    */
    std::string systemDateTime() const
    {
        time_t rawtime;
        struct tm *timeinfo;
        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);
        printf("Current local time and date: %d%s%d%s%d%s%d%s%d%s%d",
               timeinfo->tm_mday, "/", timeinfo->tm_mon, "/", 1900 + timeinfo->tm_year, "/  ",
               timeinfo->tm_hour, ":", timeinfo->tm_min, ":", timeinfo->tm_sec);
    }
};

/*!
    \fn bool operator==(dateTime &x, dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime per uguaglianza tramite la stringa \c dateT.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se le stringhe \c dateT sono uguali e gli oggetti sono distinti, \c false altrimenti.

    \warning Poiché \c dateT non viene mai popolata, questo operatore restituirà
    sempre \c true per oggetti distinti (due stringhe vuote sono uguali).
    Considerare il confronto tramite timestamp o campi individuali.
*/
bool operator==(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() == y.getDateTime()) return true;
    return false;
}

/*!
    \fn bool operator>(dateTime &x, dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime: verifica se il primo è successivo al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se la stringa \c dateT di \a x è lessicograficamente maggiore di quella di \a y.

    \warning Il confronto è lessicografico su stringhe. Poiché \c dateT non viene popolata,
    il risultato è sempre \c false. Utilizzare un confronto numerico su timestamp o campi data/ora.
*/
bool operator>(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() > y.getDateTime()) return true;
    return false;
}

/*!
    \fn bool operator<(dateTime &x, dateTime &y)
    \relates dateTime
    \brief Confronta due oggetti dateTime: verifica se il primo è precedente al secondo.
    \param x Primo operando.
    \param y Secondo operando.
    \return \c true se la stringa \c dateT di \a x è lessicograficamente minore di quella di \a y.

    \warning Stessa limitazione di operator>: il confronto su \c dateT vuota è sempre \c false.
*/
bool operator<(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() < y.getDateTime()) return true;
    return false;
}