#ifndef DATAORA_H
#define DATAORA_H

#pragma once

#include <string>
#include <ctime>

// ==================== ECCEZIONI ====================
class orarioexp
{
public:
    std::string errore;
    int str;
    orarioexp(std::string s, int st);
};

class dataexp
{
public:
    std::string errore;
    int str;
    dataexp(std::string s, int st);
};

class mesexp
{
public:
    std::string errore;
    int str;
    mesexp(std::string s, int st);
};

class annoexp
{
public:
    std::string errore;
    int str;
    annoexp(std::string s, int st);
};

class DateTimexp
{
public:
    std::string errore;
    DateTimexp(std::string s);
};

// ==================== CLASSE ORARIO ====================
class orario
{
private:
    int timestamp;
    int sc, min, hr;
    void create_timestamp(int h, int m, int s);

public:
    orario(int s = 0, int m = 0, int h = 0, int tm = 0);

    int getSec() const;
    int getMin() const;
    int getHour() const;
    int getTimestamp() const;
    std::string curTime() const;
    void modificaOrario(int h = -1, int m = -1, int s = -1);


    static int systemHour();
    static int systemSecond();
    static int systemMin();
    static int systemTime();

    friend bool operator==(const orario &x, const orario &y);
    friend bool operator>(const orario &x, const orario &y);
    friend bool operator<(const orario &x, const orario &y);

    orario& operator=(const orario &y);
};

// ==================== CLASSE DATA ====================
class data
{
private:
    static const std::string settimana[7];
    int st;
    std::string stcor;

public:
    data(int x = 0);

    std::string getGiorno() const;
    int getDate() const;
    void modificaData(int x);

    static int systemDay();

    int ConfGiorni(int x) const;
    data &operator=(const data &y);

    friend bool operator==(const data &x, const data &y);
    friend bool operator>(const data &x, const data &y);
    friend bool operator<(const data &x, const data &y);
};

// ==================== CLASSE MESE ====================
class mese
{
private:
    static const std::string mesi[12];
    int ms;
    std::string mscor;

public:
    mese(int x = 0);

    int numMese() const;
    std::string getMese() const;
    void modificaMese(int x);

    static int systemMonth();

    int ConfMesi(int x) const;
    mese &operator=(const mese &y);

    friend bool operator==(const mese &x, const mese &y);
    friend bool operator>(const mese &x, const mese &y);
    friend bool operator<(const mese &x, const mese &y);
};

// ==================== CLASSE ANNO ====================
class anno
{
private:
    int annocr;

public:
    anno(int an = systemYear());

    int getAnno() const;
    void modificaAnno(int x);

    static int systemYear();

    anno &operator=(const anno &y);

    friend bool operator==(const anno &x, const anno &y);
    friend bool operator>(const anno &x, const anno &y);
    friend bool operator<(const anno &x, const anno &y);
};

// ==================== CLASSE DATETIME ====================
class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;

public:
    dateTime(int an = systemYear(), int m = systemMonth(), int d = systemDay(),
             int h = systemHour(), int mn = systemMin(), int s = systemSecond());

    std::string getDateTime() const;
    void modificaDateTime(int an = 0, int m = -1, int d = -1, int h = -1, int mn = -1, int s = -1);
    void FormatDate(std::string s = " ");
    std::string systemDateTime() const;
    time_t toTimestamp() const;
    dateTime &operator=(const dateTime &y);

    friend bool operator==(const dateTime &x, const dateTime &y);
    friend bool operator>(const dateTime &x, const dateTime &y);
    friend bool operator<(const dateTime &x, const dateTime &y);
};

// Dichiarazione della classe dataNascita (definizione in dataNascita.h)
orario orarioFromString(const std::string& s);


#endif // DATAORA_H
