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

/*
@Classe oraio rappresenatta come:
int timestamp rapprenta il timestamp completo dell'intera data
int sc rappresenta i secondi in sessantesimi
int min rappresenta i minuti in sessantesimi
int hr rappresenta le ore in sessantesimi
*/
class orario
{
private:
    int timestamp;
     int sc, min, hr;

public:
    orario(  int,  int,  int, int);
     int getSec() const;

     int getMin() const;

     int getHour() const;

    int getTimestamp () const;

    std::string curTime() const;
    void getFormat();
    void modificaOrario( int,  int,  int);
    static  int systemHour();

    static  int systemSecond();

    static  int systemMin();

    static int systemTime();
};

/*
@Classe data rappresenatta come:
stringa stcor che rappresenta il valore in stringa del giorno corrente
 int st che rappresenta in valore intero il giorno corrente
*/
class data
{
private:
    // consiglio: se volessimo stampare il nome abbreviato alle prime 3 lettere, mettere una funzione di troncamento che stampa fino alla terza lettera
    std::string Settimana[7] = {"lunedi", "martedi", "mercoledi", "giovedi", "venerdi", "sabato", "domenica"};
     int st;
    std::string stcor;

public:
    /*
    prende un intero con valore compreso da 0 e 6 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */
    data( int);

    // ritorna la data dell'oggetto corrente
    std::string getGiorno() const;

     int getDate() const;

    /*
    cambia il valore dell'oggetto data
    */
    void modificaData( int);
    static  int systemDay();

    /*
    Prende in input un'oggetto di tipo data
    Confronta i valori di due date
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
     int ConfGrioni( int) const;
};

/*
@Classe mese rappresenatta come:
stringa mscor:  rappresenta il valore in stringa del mese corrente
 int ms: rappresenta in valore intero il mese corrente
*/
class mese
{
private:
    // aggiungere funzione che passato il mese come parametro (o indice dell'enumMese) restituisce i giorni di quel mese?
    std::string Mesi[12] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};
     int ms;
    std::string mscor; // variabile che tiene la stringa del mese, con scopo di velocizzare la stampa dello stesso (valuare l'utilità)

public:
    /*
    prende un intero con valore compreso da 0 e 11 compreso e default 0;
    se il valore è valido inizializza l'oggetto;
    altrimenti lancia un'eccezzione (da gestire).
    */

    // Versione 1
    mese( int);

    // ritorna il mese dell'oggetto corrente
    std::string getMese() const;

    /*
    cambia il valore dell'oggetto mese
    */
    void modificaMese( int);
    static  int systemMonth();

    /*
    Prende in input un'oggetto di tipo mese
    Confronta i valori di due mesi
    ritorna 0 se sono uguali
    -1 se l'oggetto corretnte è maggiore
    1 se l'oggetto corrente è minore
    */

    // potrebbe essere portata esterna come overloading dell'operatore di confronto (da valutare)
     int ConfMesi( int) const;
};
/*
@Classe anno rappresenatta come:
 int annocr: rappresenta in valore intero l'anno corrente
*/
class anno
{
private:
     int annocr;

public:
    anno( int);

    // restituisce il valore dell'anno
     int getAnno() const;
    // prende in input un intero e aggiorna il valore dell'anno
    void modificaAnno( int);

    // ritorna l'anno corrente del sistema
    static int systemYear();
};

class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;
    // attributi ereditati
public:
    dateTime( int,  int,  int,  int,  int,  int);
    std::string getDateTime() const;
    
    
    // Ricontrollare questo metodo per evntuale efficentamento
    void modificaDateTime( int,  int, int,  int, int, int);

    // rifare funzione per accettare più tipologie di formattazione
    void FormatDate();

    //funzione che ritorna la data completa di sistema già formattata.
    //controlare formattazione per efficentamento
    std::string systemDateTime() const;
};