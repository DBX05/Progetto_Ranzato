#include "dataora.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

// ==================== ECCEZIONI ====================
orarioexp::orarioexp(std::string s, int st): errore(std::move(s)), str(st) {}
dataexp::dataexp(std::string s, int st): errore(std::move(s)), str(st) {}
mesexp::mesexp(std::string s, int st): errore(std::move(s)), str(st) {}
annoexp::annoexp(std::string s, int st): errore(std::move(s)), str(st) {}
DateTimexp::DateTimexp(std::string s): errore(std::move(s)) {}

// ==================== CLASSE ORARIO ====================
void orario::create_timestamp(int h, int m, int s) {
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    tm.tm_hour = h;
    tm.tm_min = m;
    tm.tm_sec = s;
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    timestamp = static_cast<int>(t);
}

orario::orario(int s, int m, int h, int tm) :
    timestamp(tm < 0 ? systemTime() : tm),
    hr(h < 0 || h > 23 ? systemHour() : h),
    min(m < 0 || m > 60 ? systemMin() : m),
    sc(s < 0 || s > 60 ? systemSecond() : s)
{
    if (h != 0 || min != 0 || s != 0)
        create_timestamp(h, m, s);
}

int orario::getSec() const { return sc; }
int orario::getMin() const { return min; }
int orario::getHour() const { return hr; }
int orario::getTimestamp() const { return timestamp; }
std::string orario::curTime() const {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << hr << ":" << std::setw(2) << min << ":" << std::setw(2) << sc;
    return ss.str();
}
void orario::modificaOrario(int h, int m, int s)
{
    if (h < 0 || h > 23) throw orarioexp("Le ore devono avere valore fra le 0 e 23", h);
    if (m < 0 || m > 60) throw orarioexp("I minuti devono avere valore fra le 0 e 60", m);
    if (s < 0 || s > 60) throw orarioexp("I secondi devono avere valore fra le 0 e 60", s);
    hr = h; min = m; sc = s;
}
int orario::systemHour()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_hour);
}
int orario::systemSecond()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_sec);
}
int orario::systemMin()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return int(localTime->tm_min);
}
int orario::systemTime()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    return static_cast<int>(std::mktime(localTime));
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
bool operator==(const orario &x, const orario &y)
{
    return x.getTimestamp() == y.getTimestamp();
}
bool operator>(const orario &x, const orario &y)
{
    return x.getTimestamp() > y.getTimestamp();
}
bool operator<(const orario &x, const orario &y)
{
    return x.getTimestamp() < y.getTimestamp();
}

// ==================== CLASSE DATA ====================
const std::string data::settimana[7] = {"lunedi","martedi","mercoledi","giovedi","venerdi","sabato","domenica"};

data::data(int x): st((x >= 0 && x < 7) ? x : -1)
{
    if (st == -1) throw dataexp("il giorno della settimana deve avere valore compreso fra 0 e 6", st);
    stcor = settimana[st];
}
std::string data::getGiorno() const { return stcor; }
int data::getDate() const { return st; }
void data::modificaData(int x)
{
    if (x < 0 || x > 6) throw dataexp("il giorno della settimana deve avere valore compreso fra 0 e 6", x);
    st = x;
    stcor = settimana[st];
}
int data::systemDay()
{
    std::time_t now = std::time(nullptr);
    std::tm *timestamp = std::localtime(&now);
    return timestamp->tm_wday;
}
int data::ConfGiorni(int x) const
{
    if (st == x) return 0;
    if (st > x) return -1;
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
bool operator==(const data &x, const data &y)
{
    return x.getDate() == y.getDate();
}
bool operator>(const data &x, const data &y)
{
    return x.getDate() > y.getDate();
}
bool operator<(const data &x, const data &y)
{
    return x.getDate() < y.getDate();
}

// ==================== CLASSE MESE ====================
const std::string mese::mesi[12] = {"gennaio","febbraio","marzo","aprile","maggio","giugno","luglio","agosto","settembre","ottobre","novembre","dicembre"};

mese::mese(int x) : ms((x >= 0 && x < 12) ? x : -1)
{
    if (ms == -1) throw mesexp("il mese deve avere valore compreso fra 0 e 11", ms);
    mscor = mesi[ms];
}
int mese::numMese() const { return ms; }
std::string mese::getMese() const { return mscor; }
void mese::modificaMese(int x) {
    if (x < 0 || x > 11) throw mesexp("il mese deve avere valore compreso fra 0 e 11", x);
    ms = x;
    mscor = mesi[ms];
}
int mese::systemMonth()
{
    std::time_t now = std::time(nullptr);
    std::tm *timestamp = std::localtime(&now);
    return timestamp->tm_mon;
}
int mese::ConfMesi(int x) const
{
    if (ms == x) return 0;
    if (ms > x) return -1;
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
bool operator==(const mese &x, const mese &y)
{
    return x.numMese() == y.numMese();
}
bool operator>(const mese &x, const mese &y)
{
    return x.numMese() > y.numMese();
}
bool operator<(const mese &x, const mese &y)
{
    return x.numMese() < y.numMese();
}

// ==================== CLASSE ANNO ====================
anno::anno(int an) : annocr((an < 0) ? systemYear() : an) {}
int anno::getAnno() const { return annocr; }
void anno::modificaAnno(int x) { if (x > -1) annocr = x; }
int anno::systemYear()
{
    std::time_t now = std::time(nullptr);
    std::tm *timestamp = std::localtime(&now);
    return timestamp->tm_year + 1900;
}
anno &anno::operator=(const anno &y)
{
    if (this != &y) annocr = y.annocr;
    return *this;
}
bool operator==(const anno &x, const anno &y)
{
    return x.getAnno() == y.getAnno();
}
bool operator>(const anno &x, const anno &y)
{
    return x.getAnno() > y.getAnno();
}
bool operator<(const anno &x, const anno &y)
{
    return x.getAnno() < y.getAnno();
}

// ==================== CLASSE DATETIME ====================
dateTime::dateTime(int an, int m, int d, int h, int mn, int s)
    : orario(s, mn, h), data(d), mese(m), anno(an) {}

std::string dateTime::getDateTime() const { return dateT; }

void dateTime::modificaDateTime(int an, int m, int d, int h, int mn, int s)
{
    if (an != 0) modificaAnno(an);
    if (m != -1) modificaMese(m);
    if (d != -1) modificaData(d);
    if (h != -1 || mn != -1 || s != -1) modificaOrario(h, mn, s);
}

void dateTime::FormatDate(std::string s)
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    tm.tm_sec = getSec();
    tm.tm_min = getMin();
    tm.tm_hour = getHour();
    tm.tm_mday = getDate();
    tm.tm_year = getAnno() - 1900;
    tm.tm_mon = numMese();
    std::ostringstream ss;
    if (s.empty()) ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    else ss << std::put_time(&tm, s.c_str());
    dateT = ss.str();
}

std::string dateTime::systemDateTime() const
{
    time_t rawtime;
    struct tm *timeinfo;
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return std::string(buffer);
}

dateTime &dateTime::operator=(const dateTime &y) {
    if (this != &y) dateT = y.dateT;
    return *this;
}

bool operator==(const dateTime &x, const dateTime &y)
{
    return x.getDateTime() == y.getDateTime();
}
bool operator>(const dateTime &x, const dateTime &y)
{
    return x.getDateTime() > y.getDateTime();
}
bool operator<(const dateTime &x, const dateTime &y)
{
    return x.getDateTime() < y.getDateTime();
}
