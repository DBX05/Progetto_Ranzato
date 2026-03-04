#include <string>
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
    uint32_t secondi;

public:
    orario();
    uint32_t getSec() const;
    // metodi void: cambiare il tipo di ritorno con quello adeguato se necessario
    void getFormat();
    void modificaOrario();
    void systemTime() const;
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
    // Possibile aggiunta futura: void systemDay() const;

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
    // Possibile aggiunta futura : void systemMonth() const;

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