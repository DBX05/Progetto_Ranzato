#include <string>
#include <ctime>

// ==================== ECCEZIONI ====================

/* orarioexp
 * CLASSE: orarioexp
 * Descrizione: Eccezione per errori di validazione dell'orario
 * 
 * ATTRIBUTI PUBLIC:
 *   - std::string errore: messaggio descrittivo dell'errore
 *   - int str: valore che ha causato l'errore
 */
class orarioexp
{
public:
    std::string errore;
    int str;

    // Costruttore: crea eccezione di orario
    // Input: const string& s (messaggio errore), int st (valore errato)
    // Output: eccezione inizializzata
    orarioexp(std::string s, int st);
}; // fine classe orarioexp

/* dataexp
 * CLASSE: dataexp
 * Descrizione: Eccezione per errori di validazione del giorno della settimana
 * 
 * ATTRIBUTI PUBLIC:
 *   - std::string errore: messaggio descrittivo dell'errore
 *   - int str: valore che ha causato l'errore
 */
class dataexp
{
public:
    std::string errore;
    int str;

    // Costruttore: crea eccezione di data
    // Input: const string& s (messaggio errore), int st (valore errato)
    // Output: eccezione inizializzata
    dataexp(std::string s, int st);
}; // fine classe dataexp

/* mesexp
 * CLASSE: mesexp
 * Descrizione: Eccezione per errori di validazione del mese
 * 
 * ATTRIBUTI PUBLIC:
 *   - std::string errore: messaggio descrittivo dell'errore
 *   - int str: valore che ha causato l'errore
 */
class mesexp
{
public:
    std::string errore;
    int str;

    // Costruttore: crea eccezione di mese
    // Input: const string& s (messaggio errore), int st (valore errato)
    // Output: eccezione inizializzata
    mesexp(std::string s, int st);
}; // fine classe mesexp

/* annoexp
 * CLASSE: annoexp
 * Descrizione: Eccezione per errori di validazione dell'anno
 * 
 * ATTRIBUTI PUBLIC:
 *   - std::string errore: messaggio descrittivo dell'errore
 *   - int str: valore che ha causato l'errore
 */
class annoexp
{
public:
    std::string errore;
    int str;

    // Costruttore: crea eccezione di anno
    // Input: const string& s (messaggio errore), int st (valore errato)
    // Output: eccezione inizializzata
    annoexp(std::string s, int st);
}; // fine classe annoexp

/* DateTimexp
 * CLASSE: DateTimexp
 * Descrizione: Eccezione per errori di validazione di data-ora
 * 
 * ATTRIBUTI PUBLIC:
 *   - std::string errore: messaggio descrittivo dell'errore
 */
class DateTimexp
{
public:
    std::string errore;

    // Costruttore: crea eccezione di dateTime
    // Input: const string& s (messaggio errore)
    // Output: eccezione inizializzata
    DateTimexp(std::string s);
}; // fine classe DateTimexp


// ==================== CLASSE ORARIO ====================

/* orario
 * CLASSE: orario
 * Descrizione: Rappresenta un orario composto da ore, minuti e secondi con timestamp Unix
 * 
 * ATTRIBUTI PRIVATE:
 *   - int timestamp: timestamp Unix completo dell'orario
 *   - int sc: secondi (0-59)
 *   - int min: minuti (0-59)
 *   - int hr: ore (0-23)
 * 
 * METODI:
 *   - orario(int, int, int, int): costruttore
 *   - getSec/getMin/getHour/getTimestamp: getter
 *   - curTime: restituisce orario formattato
 *   - modificaOrario: modifica orario con validazione
 *   - Metodi statici system*: ottenere valori di sistema
 *   - Operatori di confronto: ==, >, <
 */
class orario
{
private:
    int timestamp;
    int sc, min, hr;

    // Crea il timestamp Unix basato su ore, minuti, secondi
    // Input: int h (ore), int m (minuti), int s (secondi)
    // Output: aggiorna timestamp
    void create_timestamp(int h, int m, int s);

public:

    // Costruttore: inizializza orario
    // Input: int s (secondi, default 0), int m (minuti, default 0), int h (ore, default 0), int tm (timestamp, default 0)
    // Output: oggetto orario inizializzato (valori negativi -> valori sistema)
    orario(int s = 0, int m = 0, int h = 0, int tm = 0);

    // Getter: restituisce i secondi
    // Input: nessuno
    // Output: int secondi (0-60)
    int getSec() const;

    // Getter: restituisce i minuti
    // Input: nessuno
    // Output: int minuti (0-60)
    int getMin() const;

    // Getter: restituisce le ore
    // Input: nessuno
    // Output: int ore (0-23)
    int getHour() const;

    // Getter: restituisce il timestamp Unix
    // Input: nessuno
    // Output: int timestamp
    int getTimestamp() const;

    // Getter: restituisce l'orario formattato come stringa (HH:MM:SS)
    // Input: nessuno
    // Output: string formato "HH:MM:SS"
    std::string curTime() const;

    // Modifica l'orario con validazione dei valori
    // Input: int h (ore 0-23, default -1=non modifica), int m (minuti 0-60, default -1),
    //        int s (secondi 0-60, default -1)
    // Output: aggiorna valori se validi
    // Eccezioni: orarioexp se valori non validi
    void modificaOrario(int h = -1, int m = -1, int s = -1);

    // Getter statico: restituisce l'ora corrente di sistema
    // Input: nessuno
    // Output: int ora corrente (0-23)
    static int systemHour();

    // Getter statico: restituisce i secondi correnti di sistema
    // Input: nessuno
    // Output: int secondi correnti (0-60)
    static int systemSecond();

    // Getter statico: restituisce i minuti correnti di sistema
    // Input: nessuno
    // Output: int minuti correnti (0-60)
    static int systemMin();

    // Getter statico: restituisce il timestamp Unix corrente
    // Input: nessuno
    // Output: int timestamp
    static int systemTime();

    // Operatori di confronto
    friend bool operator==(const orario &x, const orario &y);  // Uguaglianza
    friend bool operator>(const orario &x, const orario &y);   // Maggior di
    friend bool operator<(const orario &x, const orario &y);   // Minor di

    // Assegnamento (copy assignment)
    orario& operator=(const orario &y);

}; // fine classe orario

// ==================== CLASSE DATA ====================

/* data
 * CLASSE: data
 * Descrizione: Rappresenta un giorno della settimana come intero e stringa
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string Settimana[7]: array giorni della settimana
 *   - int st: giorno come intero (0-6)
 *   - std::string stcor: giorno come stringa
 * 
 * METODI:
 *   - data(int): costruttore
 *   - getGiorno: restituisce nome giorno
 *   - getDate: restituisce indice giorno
 *   - modificaData: cambia giorno con validazione
 *   - systemDay: ottiene giorno di sistema
 *   - ConfGiorni: confronta due giorni
 *   - Operatori: =, ==, >, <
 */
class data
{
private:
    static const std::string settimana[7];
    int st;
    std::string stcor;

public:
    // Costruttore: inizializza giorno della settimana
    // Input: int x (giorno 0-6, default 0 = lunedi)
    // Output: oggetto data inizializzato
    // Eccezioni: dataexp se valore non compreso tra 0-6
    data(int x = 0);

    // Getter: restituisce il nome del giorno della settimana
    // Input: nessuno
    // Output: string nome giorno (es. "lunedi", "martedi", ecc.)
    std::string getGiorno() const;

    // Getter: restituisce l'indice del giorno (0-6)
    // Input: nessuno
    // Output: int indice giorno
    int getDate() const;

    // Modifica il giorno della settimana con validazione
    // Input: int x (giorno 0-6)
    // Output: aggiorna st e stcor se valido
    // Eccezioni: dataexp se valore non compreso tra 0-6
    void modificaData(int x);

    // Getter statico: restituisce il giorno della settimana corrente di sistema
    // Input: nessuno
    // Output: int giorno corrente (0=domenica, 1=lunedi, ecc.)
    static int systemDay();

    // Confronta il giorno corrente con un altro giorno
    // Input: int x (giorno da confrontare 0-6)
    // Output: int (0=uguali, -1=questo > x, 1=questo < x)
    int ConfGiorni(int x) const;

    // Operatore di assegnamento
    data &operator=(const data &y);

    // Operatori di confronto
    friend bool operator==(const data &x, const data &y);  // Uguaglianza
    friend bool operator>(const data &x, const data &y);   // Maggior di
    friend bool operator<(const data &x, const data &y);   // Minor di
}; // fine classe data

// ==================== CLASSE MESE ====================

/* mese
 * CLASSE: mese
 * Descrizione: Rappresenta un mese dell'anno come intero e stringa
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string Mesi[12]: array mesi dell'anno
 *   - int ms: mese come intero (0-11)
 *   - std::string mscor: mese come stringa
 * 
 * METODI:
 *   - mese(int): costruttore
 *   - numMese: restituisce indice mese
 *   - getMese: restituisce nome mese
 *   - modificaMese: cambia mese con validazione
 *   - systemMonth: ottiene mese di sistema
 *   - ConfMesi: confronta due mesi
 *   - Operatori: =, ==, >, <
 */
class mese
{
private:
    static const std::string mesi[12];
    int ms;
    std::string mscor;

public:
    // Costruttore: inizializza mese dell'anno
    // Input: int x (mese 0-11, default 0 = gennaio)
    // Output: oggetto mese inizializzato
    // Eccezioni: mesexp se valore non compreso tra 0-11
    mese(int x = 0);

    // Getter: restituisce l'indice numerico del mese (0-11)
    // Input: nessuno
    // Output: int indice mese
    int numMese() const;

    // Getter: restituisce il nome del mese
    // Input: nessuno
    // Output: string nome mese (es. "gennaio", "febbraio", ecc.)
    std::string getMese() const;

    // Modifica il mese con validazione
    // Input: int x (mese 0-11)
    // Output: aggiorna ms e mscor se valido
    // Eccezioni: mesexp se valore non compreso tra 0-11
    void modificaMese(int x);

    // Getter statico: restituisce il mese corrente di sistema
    // Input: nessuno
    // Output: int mese corrente (0=gennaio, 11=dicembre)
    static int systemMonth();

    // Confronta il mese corrente con un altro mese
    // Input: int x (mese da confrontare 0-11)
    // Output: int (0=uguali, -1=questo > x, 1=questo < x)
    int ConfMesi(int x) const;

    // Operatore di assegnamento
    mese &operator=(const mese &y);

    // Operatori di confronto
    friend bool operator==(const mese &x, const mese &y);  // Uguaglianza
    friend bool operator>(const mese &x, const mese &y);   // Maggior di
    friend bool operator<(const mese &x, const mese &y);   // Minor di
}; // fine classe mese

// ==================== CLASSE ANNO ====================

/* anno
 * CLASSE: anno
 * Descrizione: Rappresenta un anno come valore intero
 * 
 * ATTRIBUTI PRIVATE:
 *   - int annocr: anno come intero
 * 
 * METODI:
 *   - anno(int): costruttore
 *   - getAnno: restituisce l'anno
 *   - modificaAnno: modifica l'anno
 *   - systemYear: ottiene anno di sistema
 *   - Operatori: =, ==, >, <
 */
class anno
{
private:
    int annocr;

public:
    // Costruttore: inizializza anno
    // Input: int an (anno, default = anno di sistema)
    // Output: oggetto anno inizializzato (valore negativo -> anno di sistema)
    anno(int an = systemYear());

    // Getter: restituisce il valore dell'anno
    // Input: nessuno
    // Output: int anno
    int getAnno() const;

    // Modifica l'anno
    // Input: int x (anno, deve essere > 0)
    // Output: aggiorna annocr se valido (> -1)
    void modificaAnno(int x);

    // Getter statico: restituisce l'anno corrente di sistema
    // Input: nessuno
    // Output: int anno corrente
    static int systemYear();

    // Operatore di assegnamento
    anno &operator=(const anno &y);

    // Operatori di confronto
    friend bool operator==(const anno &x, const anno &y);  // Uguaglianza
    friend bool operator>(const anno &x, const anno &y);   // Maggior di
    friend bool operator<(const anno &x, const anno &y);   // Minor di
}; // fine classe anno

// ==================== CLASSE DATETIME ====================

/* dateTime
 * CLASSE: dateTime
 * Descrizione: Rappresenta una data e ora completa tramite ereditarietà multipla
 * Estende: orario, data, mese, anno
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string dateT: stringa rappresentazione data-ora
 *   - Attributi ereditati da orario, data, mese, anno
 * 
 * METODI:
 *   - dateTime(...): costruttore
 *   - getDateTime: restituisce data-ora formattata
 *   - modificaDateTime: modifica uno o più componenti
 *   - FormatDate: formatta data secondo pattern specificato
 *   - systemDateTime: restituisce data-ora di sistema
 *   - Operatori: =, ==, >, <
 */
class dateTime : public orario, public data, public mese, public anno
{
private:
    std::string dateT;

public:
    // Costruttore: inizializza data-ora completa
    // Input: int an (anno, default = anno di sistema), int m (mese 0-11, default = mese sistema),
    //        int d (giorno 0-6, default = giorno sistema), int h (ore 0-23, default = ora sistema),
    //        int mn (minuti 0-60, default = minuti sistema), int s (secondi 0-60, default = secondi sistema)
    // Output: oggetto dateTime inizializzato
    dateTime(int an = systemYear(), int m = systemMonth(), int d = systemDay(),
             int h = systemHour(), int mn = systemMin(), int s = systemSecond());

    // Getter: restituisce la data-ora formattata come stringa
    // Input: nessuno
    // Output: string data-ora formattata (giorno/mese/anno ore:minuti:secondi)
    std::string getDateTime() const;

    // Modifica uno o più componenti della data-ora con validazione
    // Input: int an (anno, 0=non modifica), int m (mese -1=non modifica), int d (giorno -1),
    //        int h (ore -1), int mn (minuti -1), int s (secondi -1)
    // Output: aggiorna componenti se validi
    // Eccezioni: eccezioni specifiche se valori non validi
    void modificaDateTime(int an = 0, int m = -1, int d = -1, int h = -1, int mn = -1, int s = -1);

    // Formatta la data-ora secondo il pattern specificato
    // Input: const string& s (pattern di formato, vuoto = formato default "YYYY-MM-DD HH:MM:SS")
    // Output: stringa formattata secondo il pattern (es. "DD.MM.YYYY hh:mm:ss")
    void FormatDate(std::string s = " ");

    // Ottiene e stampa la data-ora corrente di sistema
    // Input: nessuno
    // Output: stringa data-ora di sistema (formato: giorno/mese/anno ore:minuti:secondi)
    std::string systemDateTime() const;

    // Operatore di assegnamento
    dateTime &operator=(const dateTime &y);

    // Operatori di confronto
    friend bool operator==(const dateTime &x, const dateTime &y);  // Uguaglianza
    friend bool operator>(const dateTime &x, const dateTime &y);   // Maggior di
    friend bool operator<(const dateTime &x, const dateTime &y);   // Minor di

}; // fine classe dateTime

// ==================== CLASSE DATANASCITA ====================

/* dataNascita
 * CLASSE: dataNascita
 * Descrizione: Rappresenta una data di nascita (data completa senza orario) con validazione robusta
 * Estende: data, mese, anno
 * 
 * METODI:
 *   - dataNascita(int, int, int): costruttore con giorno/mese/anno
 *   - dataNascita(string): costruttore con stringa
 *   - getDataFormatted: restituisce data formattata
 *   - getEta: calcola età attuale
 *   - isValidBirthDate: valida se data è corretta
 *   - Operatori: ==, >, <
 */
class dataNascita : public data, public mese, public anno
{
public:
    
    // Costruttore di default rimosso (deleted)
    dataNascita() = delete;

    // Costruttore: inizializza data di nascita con validazione
    // Input: int giorno (1-31), int meseVal (1-12), int annoVal (anno > 1900)
    // Output: oggetto dataNascita inizializzato
    // Eccezioni: invalid_argument se data non valida
    dataNascita(int giorno, int meseVal, int annoVal);

    // Costruttore: inizializza data di nascita da stringa nel formato YYYY-MM-DD
    // Input: const string& dateString (formato "YYYY-MM-DD")
    // Output: oggetto dataNascita inizializzato
    // Eccezioni: invalid_argument se stringa non nel formato corretto o data non valida
    dataNascita(const std::string& dateString);

    // Getter: restituisce la data di nascita formattata come stringa
    // Input: nessuno
    // Output: string data formattata (GG/MM/AAAA)
    std::string getDataFormatted() const;

    // Getter: calcola l'età attuale della persona basata sulla data di nascita
    // Input: nessuno
    // Output: int età in anni
    int getEta() const;

    // Valida se la data di nascita è corretta e logicamente valida
    // Input: nessuno
    // Output: bool (true=valida, false=non valida)
    bool isValidBirthDate() const;

    // Operatori di confronto
    friend bool operator==(const dataNascita &x, const dataNascita &y);  // Uguaglianza
    friend bool operator>(const dataNascita &x, const dataNascita &y);   // Maggior di
    friend bool operator<(const dataNascita &x, const dataNascita &y);   // Minor di

}; // fine classe dataNascita
