#include <string>
#include <ctime>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <dataora.h>

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
@Classe orario rappresentata come:
int timestamp rapprenta il timestamp completo dell'intera data
int sc rappresenta i secondi in sessantesimi
int min rappresenta i minuti in sessantesimi
int hr rappresenta le ore in sessantesimi
*/

// ==================== ECCEZIONI ====================
orarioexp::orarioexp(std::string s, int st): errore(s), str(st) {};
dataexp::dataexp(std::string s, int st): errore(s), str(st) {}
mesexp::mesexp(std::string s, int st): errore(s), str(st) {}
annoexp::annoexp(std::string s, int st): errore(s), str(st) {};
DateTimexp::DateTimexp(std::string s): errore(s) {};



// ==================== CLASSE ORARIO ====================

void orario::create_timestamp( int h,  int m,  int s) {
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
orario::orario(int s, int m, int h, int tm) : timestamp(tm < 0 ? systemTime() : tm), hr(h < 0 || h > 23 ? systemHour() : h),
                                                          min(m < 0 || m > 60 ? systemMin() : m), sc(s < 0 || s > 60 ? systemSecond() : s)
    {
        if (h != 0 && min != 0 && s != 0)
            create_timestamp(h, m, s);
    }
int orario::getSec() const { return sc; }
int orario::getMin() const { return min; }
int orario::getHour() const { return hr; }
int orario::getTimestamp() const { return timestamp; }
std::string orario::curTime() const { return std::to_string(hr) + ":" + std::to_string(min) + ":" + std::to_string(sc); }
void orario::modificaOrario(int h, int m, int s)
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
int orario::systemHour()
{
    // Ottieni il timestamp corrente
    std::time_t now = std::time(nullptr);

    // Converti in struttura tm locale
    std::tm *localTime = std::localtime(&now);

    return int(localTime->tm_hour);
};
int orario::systemSecond()
{
    // Ottieni il timestamp corrente
    std::time_t now = std::time(nullptr);

    // Converti in struttura tm locale
    std::tm *localTime = std::localtime(&now);

    return int(localTime->tm_sec);
};
int orario::systemMin()
    {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm *localTime = std::localtime(&now);

        return int(localTime->tm_min);
    };
int orario::systemTime()
{
    // Ottieni il timestamp corrente
    std::time_t now = std::time(nullptr);

    // Converti in struttura tm locale
    std::tm *localTime = std::localtime(&now);

    return int(localTime);
}
orario& orario::operator=(const orario &y)
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

// ==================== CLASSE DATA ====================

const std::string settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
data::data(int x): st((x - 1 > -1 || x - 1 < 8) ? x : -1)
{
    if (st == -1) throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", st);
    stcor = settimana[st];
}
std::string data::getGiorno() const { return settimana[st]; }
int data::getDate() const { return st; };
void data::modificaData(int x)
{
    if (x < 0 || x > 7)
        throw dataexp("il giorno della settimana deve avere valore compreso fra 1 e 7", x);
    st = x;
    stcor = settimana[st];
}
int data::systemDay()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return (timestamp->tm_wday);
};
int data::ConfGiorni(int x) const
{
    if (st == x)
        return 0;
    if (st > x)
        return -1;
    return 1;
}
data &data::operator=(const data &y)
    {
        if (this != &y)
        {
            st = y.st;
            stcor = y.stcor;
        }
        return *this;
    }
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

// ==================== CLASSE MESE ====================

const std::string mese::mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
mese::mese(int x) : ms((x - 1 < 12 || x - 1 > -1) ? x : -1)
    {
        if (ms == -1)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12",ms);
        mscor = mesi[ms];
    };
int mese::numMese() const { return ms; }
std::string mese::getMese() const { return mesi[ms]; }
void mese::modificaMese(int x) {
        if (x < 0 || x > 11)
            throw mesexp("il mese deve avere valore compreso fra 1 e 12", ms);
        ms = x;
        mscor = mesi[ms];
}
int mese::systemMonth()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return timestamp->tm_mon;
}
int mese::ConfMesi(int x) const
{
    if (ms == x)
        return 0;
    if (ms > x)
        return -1;
    return 1;
}
mese &mese::operator=(const mese &y)
    {
        if (this != &y)
        {
            ms = y.ms;
            mscor = y.mscor;
        }
        return *this;
    }
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

// ==================== CLASSE ANNO ====================
anno::anno(int an) : annocr((an < 0) ? systemYear() : an) {}
int anno::getAnno() const { return annocr; }
void anno::modificaAnno(int x) { 
    if (x > -1)
        annocr = x;
}
int anno::systemYear()
{
    time_t now = std::time(nullptr);
    tm *timestamp = std::localtime(&now);
    return timestamp->tm_year;
}
anno &anno::operator=(const anno &y)
{
    if (this != &y) annocr = y.annocr;
    return *this;
}
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

// ==================== CLASSE DATETIME ====================
dateTime::dateTime( int an,  int m,  int d,  int h,  int mn,  int s): orario(s, mn, h), data(d), mese(m), anno(an) {}
std::string dateTime::getDateTime() const { return dateT; }
void dateTime::modificaDateTime(int an, int m, int d, int h, int mn, int s)
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
void dateTime::FormatDate(std::string s)
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
std::string dateTime::systemDateTime() const
{
    time_t rawtime;
    struct tm *timeinfo;

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    printf("Current local time and date: %d%s%d%s%d%s%d%s%d%s%d", timeinfo->tm_mday, "/", timeinfo->tm_mon, "/", 1900 + timeinfo->tm_year, "/  ",
           timeinfo->tm_hour, ":", timeinfo->tm_min, ":", timeinfo->tm_sec);
}
dateTime &dateTime::operator=(const dateTime &y) {
    if (this != &y) dateT = y.dateT; 
    return *this;
}
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