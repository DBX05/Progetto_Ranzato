#include <string>
#include <ctime>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

// #include <QDateTimeEdit>

/*
    File header gerarchia dataora
        Prima stesura: 2 / 2 / 2026

    Controllare:
        correttezza campi private e campi public.
        Vogliamo garantire information hiding?
    Finire:
        costruttori
*/

/*
    DOMANDE:
        1) metodi system: implementati con librerie di sistema operativo?
*/

/*
@Classe orario rappresentata come:
int timestamp rapprenta il timestamp completo dell'intera data
int sc rappresenta i secondi in sessantesimi
int min rappresenta i minuti in sessantesimi
int hr rappresenta le ore in sessantesimi
*/

class orarioexp
{
public:
    std::string errore;
    int str;
    orarioexp(std::string s, int st) : errore(s), str(st) {};
};

class orario
{
private:
    int timestamp;
     int sc, min, hr;

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

    void create_timestamp( int h,  int m,  int s)
    {
        if (timestamp != 0 || timestamp != systemTime())
        {
            std::time_t now = std::time(nullptr);
            std::tm tm = *std::localtime(&now); // prende anno/mese/giorno correnti in locale
            tm.tm_hour = h;
            tm.tm_min = m;
            tm.tm_sec = s;
            tm.tm_isdst = -1;                 // lascia che mktime determini DST
            std::time_t t = std::mktime(&tm); // interpreta tm come ora locale
            timestamp = static_cast<long long int>(t);
        }
    }

public:
    orario(int s = 0, int m = 0, int h = 0, int tm = 0) : timestamp(tm < 0 ? systemTime() : tm), hr(h < 0 || h > 23 ? systemHour() : h),
                                                          min(m < 0 || m > 60 ? systemMin() : m), sc(s < 0 || s > 60 ? systemSecond() : s)
    {
        if (h != 0 && min != 0 && s != 0)
            create_timestamp(h, m, s);
    }
<<<<<<< HEAD:fileCpp/dataora/data.cpp

    orario(std::string ora)
    {
        
        /*if (h != 0 && min != 0 && s != 0)
            create_timestamp(h, m, s);*/
    }
    unsigned int getSec() const
=======
     int getSec() const
>>>>>>> 456458f5ac9cf22659e907edd1c0d71a6c65528e:fileCpp/dataora/dataora.cpp
    {
        return sc;
    };

     int getMin() const
    {
        return min;
    };

     int getHour() const
    {
        return hr;
    };

    int getTimestamp() const
    {
        return timestamp;
    };

    std::string getorario() const{
        return std::to_string(hr) + ":" + std::to_string(min) + ":" + std::to_string(sc);
    }

    std::string curTime() const
    {
        return std::to_string(hr) + "/" + std::to_string(min) + "/" + std::to_string(sc);
    };
    void getFormat();
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
    static  int systemHour()
    {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm *localTime = std::localtime(&now);

        return int(localTime->tm_hour);
    };

    static  int systemSecond()
    {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm *localTime = std::localtime(&now);

        return int(localTime->tm_sec);
    };

    static  int systemMin()
    {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm *localTime = std::localtime(&now);

        return int(localTime->tm_min);
    };

    static int systemTime()
    {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm *localTime = std::localtime(&now);

        // Estrai l'ora e formattala come stringa
        // char buffer[3]; // 2 cifre + terminatore
        // std::strftime(buffer, sizeof(buffer), "%H", localTime);

        return int(localTime);
    };
};

bool operator==(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() == y.getTimestamp())
        return true;
    return false;
}

bool operator>(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() > y.getTimestamp())
        return true;
    return false;
}

bool operator<(orario &x, orario &y)
{
    if (&x != &y && x.getTimestamp() < y.getTimestamp())
        return true;
    return false;
}

/*
@Classe data rappresenatta come:
stringa stcor che rappresenta il valore in stringa del giorno corrente
 int st che rappresenta in valore intero il giorno corrente
*/

class dataexp
{
public:
    std::string errore;
    int str;
    dataexp(std::string s, int st) : errore(s), str(st) {};
};
class data
{
private:
    // consiglio: se volessimo stampare il nome abbreviato alle prime 3 lettere, mettere una funzione di troncamento che stampa fino alla terza lettera
    std::string Settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
    int st;
    std::string stcor;

    data &operator=(data &y)
    {
        if (this != &y)
        {
            st = y.st;
            stcor = y.stcor;
        }
        return *this;
    }

public:
    /*
    prende un intero con valore compreso da 0 e 6 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */
    data(int x = 0) : st((x - 1 > -1 || x - 1 < 8) ? x : -1)
    {
        if (st == -1)
            throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", st);
        stcor = Settimana[st];
    };

    // ritorna la data dell'oggetto corrente
    std::string getGiorno() const
    {
        return Settimana[st];
    };

    int getDate() const
    {
        return st;
    };

    /*
    cambia il valore dell'oggetto data
    */
    void modificaData(int x)
    {
        if (x < 0 || x > 7)
            throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", x);
        st = x;
        stcor = Settimana[st];
    };
    static  int systemDay()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return (timestamp->tm_wday);
    };

    /*
    Prende in input un'oggetto di tipo data
    Confronta i valori di due date
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
    int ConfGrioni(int x) const
    {
        if (st == x)
            return 0;
        if (st > x)
            return -1;
        return 1;
    }
};

bool operator==(data &x, data &y)
{
    if (&x != &y && x.getDate() == y.getDate())
        return true;
    return false;
}

bool operator>(data &x, data &y)
{
    if (&x != &y && x.getDate() > y.getDate())
        return true;
    return false;
}

bool operator<(data &x, data &y)
{
    if (&x != &y && x.getDate() < y.getDate())
        return true;
    return false;
}

/*
@Classe mese rappresenatta come:
stringa mscor:  rappresenta il valore in stringa del mese corrente
 int ms: rappresenta in valore intero il mese corrente
*/

class mesexp
{
public:
    std::string errore;
    int str;
    mesexp(std::string s, int st) : errore(s), str(st) {};
};
class mese
{
private:
    // aggiungere funzione che passato il mese come parametro (o indice dell'enumMese) restituisce i giorni di quel mese?
    std::string Mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
     int ms;
    std::string mscor; // variabile che tiene la stringa del mese, con scopo di velocizzare la stampa dello stesso (valuare l'utilità)

    mese &operator=(mese &y)
    {
        if (this != &y)
        {
            ms = y.ms;
            mscor = y.mscor;
        }
        return *this;
    }

public:
    /*
    prende un intero con valore compreso da 0 e 11 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */

    // Versione 1
    // controllare assegnazione a ms: se x fosse per esempio 14, la prima condizione dell'or è falsa,
    // ma la seconda è vera, quindi assegno 14 a ms e alla riga 232 assegno mscor = Mesi[14] ! out of bounds
    // va messo && ?
    mese( int x = 0) : ms((x - 1 < 12 || x - 1 > -1) ? x : -1)
    {
        if (ms == -1)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12",ms);
        mscor = Mesi[ms];
    };

    unsigned int numMese() const
    {
        return ms;
    }

    // ritorna il mese dell'oggetto corrente
    std::string getMese() const
    {
        return Mesi[ms];
    };

    /*
    cambia il valore dell'oggetto mese
    */
    void modificaMese(int x)
    {
        if (x < 0 || x > 11)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12", ms);
        ms = x;
        mscor = Mesi[ms];
    }
    static  int systemMonth()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return timestamp->tm_mon;
    };

    /*
    Prende in input un oggetto di tipo mese
    Confronta i valori di due mesi
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
    int ConfMesi(int x) const
    {
        if (ms == x)
            return 0;
        if (ms > x)
            return -1;
        return 1;
    }
};

bool operator==(mese &x, mese &y)
{
    if (&x != &y && x.numMese() == y.numMese())
        return true;
    return false;
}

bool operator>(mese &x, mese &y)
{
    if (&x != &y && x.numMese() > y.numMese())
        return true;
    return false;
}

bool operator<(mese &x, mese &y)
{
    if (&x != &y && x.numMese() < y.numMese())
        return true;
    return false;
}
/*
@Classe anno rappresenatta come:
 int annocr: rappresenta in valore intero l'anno corrente
*/
class annoexp
{
public:
    std::string errore;
    int str;
    annoexp(std::string s, int st) : errore(s), str(st) {};
};
class anno
{
private:
     int annocr;

    anno &operator=(anno &y)
    {
        if (this != &y)
        {
            annocr = y.annocr;
        }
        return *this;
    }

public:
    anno(int an = systemYear()) : annocr((an < 0) ? systemYear() : an) {};

    // restituisce il valore dell'anno
     int getAnno() const
    {
        return annocr;
    };
    // prende in input un intero e aggiorna il valore dell'anno
    void modificaAnno( int x)
    {
        if (x > -1)
            annocr = x;
    };

    // ritorna l'anno corrente del sistema
    static int systemYear()
    {
        time_t now = std::time(nullptr);
        tm *timestamp = std::localtime(&now);
        return timestamp->tm_year;
    }
};

bool operator==(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() == y.getAnno())
        return true;
    return false;
}

bool operator>(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() > y.getAnno())
        return true;
    return false;
}

bool operator<(anno &x, anno &y)
{
    if (&x != &y && x.getAnno() < y.getAnno())
        return true;
    return false;
}

class DateTimexp
{
public:
    std::string errore;
    DateTimexp(std::string s) : errore(s) {};
};

class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;
    // attributi ereditati

    dateTime &operator=(dateTime &y)
    {
        if (this != &y)
        {
            dateT = y.dateT;
        }
        return *this;
    }

public:
    dateTime( int an = systemYear(),  int m = systemMonth(),  int d = systemDay(),  int h = systemHour(),  int mn = systemMin(),  int s = systemSecond())
        : orario(s, mn, h), data(d), mese(m), anno(an) {};
    std::string getDateTime() const
    {
        return dateT;
    }

    // Ricontrollare funzione per eventuale modifica o ottimizzazione
    void modificaDateTime(int an = 0, int m = -1, int d = -1, int h = -1, int mn = -1, int s = -1)
    {
        if (an != 0)
            modificaAnno(an);
        if (m != -1)
            modificaMese(m);
        if (d != -1)
            modificaData(d);
        if (h != -1 || mn != -1 || s != -1)
            modificaOrario(h, mn, s);
    }

        std::string getDateTime() const{
        return getGiorno() +"/"+ getMese() +"/"+ std::to_string(getAnno()) + " " + getorario();
    }

    /*
    Prende input una stringa con valore definito a vuoto che indica il formato base;
    In caso abbia un valore diverso viene utilizzato come formato per la data;
    controlli sulla corretezza della stringa vanno fatti esternamente
    */
    void FormatDate(std::string s = "")
    {
        /*
        versione con funzione di qt:
        QDateTime dt(QDate(getAnno(), numMese(), getDate()), QTime(getHour(), getMin(), getSec()));
        */
        std::time_t t = std::time(nullptr);
        std::tm *tm_ptr = std::localtime(&t);
        tm_ptr->tm_sec = getSec();
        tm_ptr->tm_min = getMin();
        tm_ptr->tm_hour = getHour();
        tm_ptr->tm_mday = getDate();
        tm_ptr->tm_year = getAnno();
        tm_ptr->tm_mon = numMese();
        std::stringstream ss;

        if (s == "")
        {
            // QString formatted = dt.toString("dd.MM.yyyy hh:mm:ss"); // "21.05.2023 14:13:09"
            ss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
        }
        else
        {
            // QString formatted = dt.toString(s); // "21.05.2023 14:13:09"
            ss << std::put_time(tm_ptr, &s);
        }
    }

    /*
    Ritorna la data di sistema fomrattata come day/month/year hour:min:sec
    */
    std::string systemDateTime() const
    {
        time_t rawtime;
        struct tm *timeinfo;

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);
        printf("Current local time and date: %d%s%d%s%d%s%d%s%d%s%d", timeinfo->tm_mday, "/", timeinfo->tm_mon, "/", 1900 + timeinfo->tm_year, "/  ",
               timeinfo->tm_hour, ":", timeinfo->tm_min, ":", timeinfo->tm_sec);

        /*

            MOLTO OPZIONALE:

        time_t now = std::time(nullptr);
        std::tm timestamp = *std::localtime(&now);
        return getGiorno() + "/" + std::to_string(getDate()) + "/" + getMese() + "/" + std::to_string(getAnno()) + " " + std::to_string(getHour()) + ":" + std::to_string(getMin()) + ":" + std::to_string(getSec());

        --------

            Ottimale:

        time_t rawtime;
        struct tm *timeinfo;

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);
        printf("Current local time and date: %s", std::asctime(timeinfo));
        */
    };
};

bool operator==(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() == y.getDateTime())
        return true;
    return false;
}

bool operator>(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() > y.getDateTime())
        return true;
    return false;
}

bool operator<(dateTime &x, dateTime &y)
{
    if (&x != &y && x.getDateTime() < y.getDateTime())
        return true;
    return false;
}