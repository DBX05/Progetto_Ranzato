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
        enum enumGiornoSettimana{ lunedi, martedi, mercoledi, giovedi, venerdi, sabato, domenica };
    public:
        data();
        // riceve un interno compreso da 0 e 6 compreso, restituisce una stringa con il giorno della settimana
        std::string getGiornoSettimana(int) const;
        // metodi void: cambiare il tipo di ritorno con quello adeguato se necessario
        void modificaData();
        void systemDay() const;
};

class mese
{
    private:
        // aggiungere funzione che passato il mese come parametro (o indice dell'enumMese) restituisce i giorni di quel mese?
        enum enumMese{ gennaio, febbraio, marzo, aprile, maggio, giugno, luglio, agosto, settembre, ottobre, novembre, dicembre };
    public:
        mese();
        // riceve un interno compreso da 0 e 11 compreso, restituisce una stringa con il mese
        // possibile variante con secondo parametro intero: bit 0 o 1 per restituire rispettivamente stringa o indice stesso? Utile per il rispondere alla domanda sopra
        std::string getMese(int) const;
        // metodi void: cambiare il tipo di ritorno con quello adeguato se necessario
        void modificaMese();
        void systemMonth() const;

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

class dateTime: public orario, public data, public mese, public anno
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