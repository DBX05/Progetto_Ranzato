#include <string>
#include <ctime>
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

class orario
{
private:
    int timestamp, sc, min, hr;

public:
    orario(int tm = 0, int s = 0, int m = 0, int h = 0) : timestamp(tm<0 ? systemTime() : tm), hr( h<0 || h>23 ? systemHour() : h), 
    min(m<0 || m>60 ? systemMin() : m),sc(s<0 || s>60 ? systemSecond() : s){};
    int getSec() const {
        return sc;
    };

    int getMin() const {
        return min;
    };

    int getHour() const {
        return hr;
    };

    std::string curTime() const {
        return std::to_string(hr) + "/" + std::to_string(min) + "/" + std::to_string(sc);
    };
    void getFormat();
    void modificaOrario(int h = -1, int m = -1, int s = -1) {
        if(h < 0 || h > 23) throw orario();
        hr = h;
        if(m < 0 || m > 60) throw orario();
        min = m;
        if(s < 0 || s > 60) throw orario();
        sc = s;
    }
    int systemHour() const {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm* localTime = std::localtime(&now);

        return int(localTime->tm_hour);

    };

    int systemSecond() const {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm* localTime = std::localtime(&now);

        return int(localTime->tm_sec);

    };

    int systemMin() const {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm* localTime = std::localtime(&now);

        return int(localTime->tm_min);

    };

    int systemTime() const {
        // Ottieni il timestamp corrente
        std::time_t now = std::time(nullptr);

        // Converti in struttura tm locale
        std::tm* localTime = std::localtime(&now);

        // Estrai l'ora e formattala come stringa
        //char buffer[3]; // 2 cifre + terminatore
        //std::strftime(buffer, sizeof(buffer), "%H", localTime);
        
        return int(localTime);

    };
};

class data
{
private:
    // consiglio: se volessimo stampare il nome abbreviato alle prime 3 lettere, mettere una funzione di troncamento che stampa fino alla terza lettera
    std::string Settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
    unsigned int st;
    std::string stcor;

public:
    /*
    prende un intero con valore compreso da 0 e 6 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */
    data(unsigned int x = 0) : st((x > -1 || x < 8) ? x : -1)
    {
        if (st == -1)
            throw data();
        stcor = Settimana[st];
    };

    // ritorna la data dell'oggetto corrente
    std::string getGiorno(int) const
    {
        return Settimana[st];
    };

    /*
    cambia il valore dell'oggetto data
    */
    void modificaData(unsigned int x)
    {
        if (x < 0 || x > 7)
            throw data();
        st = x;
        stcor = Settimana[st];
    };
    std::string systemDay() const{
        time_t now = std::time(nullptr);
        tm* timestamp = std::localtime(&now);
        return (Settimana[timestamp->tm_wday -1]);
    };

    /*
    Prende in input un'oggetto di tipo data
    Confronta i valori di due date
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
    unsigned int ConfGrioni(unsigned int x) const
    {
        if (st == x)
            return 0;
        if (st > x)
            return -1;
        return 1;
    }
};

class mese
{
private:
    // aggiungere funzione che passato il mese come parametro (o indice dell'enumMese) restituisce i giorni di quel mese?
    std::string Mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
    unsigned int ms;
    std::string mscor; // variabile che tiene la stringa del mese, con scopo di velocizzare la stampa dello stesso (valuare l'utilità)

public:
    /*
    prende un intero con valore compreso da 0 e 11 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */

    // Versione 1
    mese(unsigned int x = 0) : ms((x < 12 || x > -1) ? x : -1)
    {
        if (ms == -1)
            throw mese();
        mscor = Mesi[ms];
    };

    // ritorna il mese dell'oggetto corrente
    std::string getMese() const
    {
        return Mesi[ms];
    };

    /*
    cambia il valore dell'oggetto mese
    */
    void modificaMese(unsigned int x)
    {
        if (x < 0 || x > 11)
            throw mese();
        ms = x;
        mscor = Mesi[ms];
    }
    std::string systemMounth() const{
        time_t now = std::time(nullptr);
        tm* timestamp = std::localtime(&now);
        return (Mesi[timestamp->tm_mon]);
    };

    /*
    Prende in input un'oggetto di tipo mese
    Confronta i valori di due mesi
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
    unsigned int ConfMesi(unsigned int x) const
    {
        if (ms == x)
            return 0;
        if (ms > x)
            return -1;
        return 1;
    }
};

class anno
{
private:
    uint32_t annoCampoDati;

public:
    anno();
    uint32_t getAnno() const;
    // metodi void: cambiare il tipo di ritorno con quello adeguato se necessario
    void modificaAnno();
    void systemYear() const;
};

class dateTime : public orario, public data, public mese, public anno
{
private:
    // attributi ereditati
public:
    dateTime();
    void getDateTime() const;
    // metodi void: cambiare il tipo di ritorno con quello adeguato se necessario
    void modificaDateTime();
    void systemDateTime() const;
};