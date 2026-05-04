#include <string>
#include <ctime>

// Exception classes

/*
    CLASSE ORARIO
    Rappresenta un orario composto da ore, minuti e secondi, con supporto a timestamp Unix.
*/

class orarioexp
{
public:
    std::string errore;
    int str;
    orarioexp(std::string s, int st);
};

class orario
{
private:
    int timestamp;
    int sc, min, hr;
    
    orario &operator=(const orario &y);
    void create_timestamp(int h, int m, int s);

public:
    orario(int s = 0, int m = 0, int h = 0, int tm = 0);
    
    int getSec() const;
    int getMin() const;
    int getHour() const;
    int getTimestamp() const;
    std::string curTime() const;
    void getFormat();
    void modificaOrario(int h = -1, int m = -1, int s = -1);
    
    static int systemHour();
    static int systemSecond();
    static int systemMin();
    static int systemTime();
    
    friend bool operator==(const orario &x, const orario &y);
    friend bool operator>(const orario &x, const orario &y);
    friend bool operator<(const orario &x, const orario &y);
};

/*
    CLASSE DATA
    Rappresenta un giorno della settimana come intero e come stringa.
*/

class dataexp
{
public:
    std::string errore;
    int str;
    dataexp(std::string s, int st);
};

class data
{
private:
    std::string Settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
    int st;
    std::string stcor;
    
    data &operator=(const data &y);

public:
    data(int x = 0);
    
    std::string getGiorno() const;
    int getDate() const;
    void modificaData(int x);
    
    static int systemDay();
    int ConfGrioni(int x) const;
    
    friend bool operator==(const data &x, const data &y);
    friend bool operator>(const data &x, const data &y);
    friend bool operator<(const data &x, const data &y);
};

/*
    CLASSE MESE
    Rappresenta un mese dell'anno come intero e come stringa.
*/

class mesexp
{
public:
    std::string errore;
    int str;
    mesexp(std::string s, int st);
};

class mese
{
private:
    std::string Mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno",
                             "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
    int ms;
    std::string mscor;
    
    mese &operator=(const mese &y);

public:
    mese(int x = 0);
    
    int numMese() const;
    std::string getMese() const;
    void modificaMese(int x);
    
    static int systemMonth();
    int ConfMesi(int x) const;
    
    friend bool operator==(const mese &x, const mese &y);
    friend bool operator>(const mese &x, const mese &y);
    friend bool operator<(const mese &x, const mese &y);
};

/*
    CLASSE ANNO
    Rappresenta un anno come valore intero.
*/

class annoexp
{
public:
    std::string errore;
    int str;
    annoexp(std::string s, int st);
};

class anno
{
private:
    int annocr;
    
    anno &operator=(const anno &y);

public:
    anno(int an = systemYear());
    
    int getAnno() const;
    void modificaAnno(int x);
    
    static int systemYear();
    
    friend bool operator==(const anno &x, const anno &y);
    friend bool operator>(const anno &x, const anno &y);
    friend bool operator<(const anno &x, const anno &y);
};

/*
    CLASSE DATETIME
    Rappresenta una data e ora completa tramite ereditarietà multipla.
*/

class DateTimexp
{
public:
    std::string errore;
    DateTimexp(std::string s);
};


class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;
    
    dateTime &operator=(const dateTime &y);

public:
    dateTime(int an = systemYear(), int m = systemMonth(), int d = systemDay(),
             int h = systemHour(), int mn = systemMin(), int s = systemSecond());
    
    std::string getDateTime() const;
    void modificaDateTime(int an = 0, int m = -1, int d = -1, int h = -1, int mn = -1, int s = -1);
    std::string FormatDate(std::string s = "");
    std::string systemDateTime() const;
    
    friend bool operator==(const dateTime &x, const dateTime &y);
    friend bool operator>(const dateTime &x, const dateTime &y);
    friend bool operator<(const dateTime &x, const dateTime &y);
};

/*
    CLASSE DATANASCITA
    Rappresenta una data di nascita (data completa senza orario).
    Eredita da data, mese e anno per validazione e gestione robusta.
*/
class dataNascita : public data, public mese, public anno
{
public:
    dataNascita() = delete;
    explicit dataNascita(int giorno, int meseVal, int annoVal);
    explicit dataNascita(const std::string& dateString);
    
    std::string getDataFormatted() const;
    int getEta() const;
    bool isValidBirthDate() const;
    
    friend bool operator==(const dataNascita &x, const dataNascita &y);
    friend bool operator>(const dataNascita &x, const dataNascita &y);
    friend bool operator<(const dataNascita &x, const dataNascita &y);
};
