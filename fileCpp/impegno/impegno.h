#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

// include personali
#include <fileCpp/dataora/dataora.h>
#include <fileCpp/persona/persona.h>

/*
    File header gerarchia impegno
        Prima stesura: 2 / 2 / 2026
    
    Gerarchia di classi per gestire impegni di vario tipo:
    - impegno (base astratta)
    - evento (impegno con orari specifici)
    - promemoria (evento con descrizione)
    - eventoLungo (evento con durata estesa)
    - raggruppa, festivita, compleanno, riunione, altroTipo (specializzazioni di eventoLungo)
    - categoriaPersona (raggruppa impegni per categoria)
*/

// ==================== PRIMO LIVELLO ====================
/*
 * CLASSE: impegno
 * Descrizione: Classe astratta base per tutti gli impegni
 * 
 * ATTRIBUTI PRIVATE:
 *   - dateTime momentoInizio: data e ora inizio dell'impegno
 *   - const int id: identificativo univoco (costante)
 *   - unsigned int priorita: livello di priorita dell'impegno
 * 
 * METODI:
 *   - virtual ~impegno(): distruttore virtuale puro
 *   - impegno(int, dateTime, unsigned int): costruttore
 *   - getMomentoInizio(): restituisce il momento di inizio
 *   - getPriorita(): restituisce il livello di priorita
 *   - getId(): restituisce l'ID dell'impegno
 *   - modificaImpegno(): aggiorna momento inizio e priorita
 *   - stampa(): stampa i dettagli (pura, implementata dalle sottoclassi)
 */
class impegno
{
    private:
        dateTime momentoInizio;
        const int id;
        unsigned int priorita;

    public:
        // Costruttore: inizializza impegno con id, momento inizio e priorita
        // Input: int id (negativo->0), dateTime momentoInizio, unsigned int priorita (negativa->0)
        // Output: oggetto impegno inizializzato
        impegno(int id, dateTime momentoInizio, unsigned int priorita);

        // Distruttore virtuale puro
        virtual ~impegno() = 0;

        // Getter: restituisce il momento di inizio dell'impegno
        // Input: nessuno
        // Output: dateTime momentoInizio
        virtual dateTime getMomentoInizio() const;

        // Getter: restituisce la priorita dell'impegno
        // Input: nessuno
        // Output: unsigned int priorita
        virtual unsigned int getPriorita() const;

        // Getter: restituisce l'ID univoco dell'impegno
        // Input: nessuno
        // Output: int id
        virtual int getId() const;

        // Modifica i parametri temporali e di priorita dell'impegno
        // Input: dateTime nuovoMomentoInizio, unsigned int nuovaPriorita
        // Output: aggiorna momentoInizio e priorita
        virtual void modificaImpegno(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita);

        // Stampa i dettagli dell'impegno a console (metodo puro)
        // Input: nessuno
        // Output: stampa a console
        virtual void stampa() const = 0;
};

// ==================== SECONDO LIVELLO ====================
/*
 * CLASSE: evento
 * Descrizione: Rappresenta un evento con orari specifici di inizio e fine
 * Estende: impegno
 * 
 * ATTRIBUTI PRIVATE:
 *   - orario inizio: orario di inizio evento
 *   - orario fine: orario di fine evento
 *   - std::string nome: nome dell'evento
 * 
 * METODI:
 *   - evento(int, dateTime, unsigned int, string, orario, orario): costruttore
 *   - getNome(): restituisce il nome
 *   - getInizio(): restituisce orario inizio formattato
 *   - getFine(): restituisce orario fine formattato
 *   - modificaName(): modifica il nome dell'evento
 *   - stampa(): stampa dettagli evento (override)
 *   - ~evento(): distruttore
 */
class evento: public impegno
{
    private:
        orario inizio;
        orario fine;
        std::string nome;

    public:
        // Costruttore: inizializza evento con id, momento, priorita, nome e orari
        // Input: int id, dateTime momentoInizio, unsigned int priorita, string nome, orario inizio, orario fine
        // Output: oggetto evento inizializzato (nome vuoto->default, inizio invalido->0:0:0, fine>inizio->0:0:0)
        evento(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine);

        // Getter: restituisce il nome dell'evento
        // Input: nessuno
        // Output: std::string nome
        std::string getNome() const;

        // Getter: restituisce l'orario di inizio formattato
        // Input: nessuno
        // Output: std::string formato orario (tramite curTime())
        std::string getInizio() const;

        // Getter: restituisce l'orario di fine formattato
        // Input: nessuno
        // Output: std::string formato orario (tramite curTime())
        std::string getFine() const;

        // Modifica il nome dell'evento (se non vuoto)
        // Input: std::string nuovoNome
        // Output: aggiorna nome se non vuoto
        void modificaName(std::string nuovoNome);

        // Stampa i dettagli dell'evento a console
        // Input: nessuno
        // Output: stampa ID, Nome, Inizio, Fine, Priorita
        void stampa() const override;

        // Distruttore
        ~evento();
};

// ==================== TERZO LIVELLO ====================
/*
 * CLASSE: promemoria
 * Descrizione: Evento con una descrizione testuale aggiuntiva
 * Estende: evento
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::string descrizione: testo descrittivo del promemoria
 * 
 * METODI:
 *   - promemoria(int, dateTime, unsigned int, string, string, orario, orario): costruttore
 *   - getDescrizione(): restituisce la descrizione
 *   - modificaPromemoria(): modifica la descrizione
 *   - stampa(): stampa promemoria completo (override)
 */
class promemoria: public evento
{
    private:
        std::string descrizione;

    public:
        // Costruttore: inizializza promemoria ereditando da evento e aggiungendo descrizione
        // Input: int id, dateTime momentoInizio, unsigned int priorita, string nome, string descrizione, orario inizio, orario fine
        // Output: oggetto promemoria inizializzato (descrizione vuota->default)
        promemoria(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, std::string descrizione, orario inizio, orario fine);

        // Getter: restituisce la descrizione del promemoria
        // Input: nessuno
        // Output: std::string descrizione
        std::string getDescrizione() const;

        // Modifica la descrizione del promemoria (se non vuota)
        // Input: std::string nuovaDescrizione
        // Output: aggiorna descrizione se non vuota
        void modificaPromemoria(std::string nuovaDescrizione);

        // Stampa i dettagli del promemoria a console
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione, Inizio, Priorita
        void stampa() const override;
};

/*
 * CLASSE: eventoLungo
 * Descrizione: Evento con durata estesa (da un momento di inizio a uno di fine)
 * Estende: evento
 * 
 * ATTRIBUTI PRIVATE:
 *   - dateTime momentoFine: data e ora di fine dell'evento
 *   - std::string Descrizione: descrizione testuale dell'evento
 * 
 * METODI:
 *   - eventoLungo(...): costruttore
 *   - getMomentoFine(): restituisce momento fine
 *   - getDescrizione(): restituisce descrizione
 *   - virtual ~eventoLungo(): distruttore puro
 *   - modificaEventoLungo(): modifica parametri dell'evento lungo
 *   - stampa(): metodo puro
 */
class eventoLungo: public evento
{
    private:
        dateTime momentoFine;
        std::string Descrizione;

    public:
        // Costruttore: inizializza evento lungo con id, momenti, priorita, nome, orari e descrizione
        // Input: int id, dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione, orario inizio, orario fine
        // Output: oggetto eventoLungo inizializzato
        eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine);

        // Getter: restituisce il momento di fine dell'evento
        // Input: nessuno
        // Output: dateTime momentoFine
        dateTime getMomentoFine() const;

        // Getter: restituisce la descrizione dell'evento lungo
        // Input: nessuno
        // Output: std::string Descrizione
        std::string getDescrizione() const;

        // Distruttore virtuale puro
        virtual ~eventoLungo() = 0;

        // Modifica parametri dell'evento lungo (momento inizio/fine, priorita, descrizione)
        // Input: dateTime nuovoMomentoInizio, unsigned int nuovaPriorita, dateTime nuovoMomentoFine, string nuovaDescrizione
        // Output: aggiorna i campi se validi (momentoFine non antecedente a inizio, descrizione non vuota)
        virtual void modificaEventoLungo(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita, dateTime nuovoMomentoFine, std::string nuovaDescrizione);

        // Stampa dettagli evento lungo (metodo puro)
        virtual void stampa() const = 0;
};

// ==================== QUARTO LIVELLO ====================
/*
 * CLASSE: raggruppa
 * Descrizione: Raggruppa più eventi in un'unica entita
 * Estende: eventoLungo
 * 
 * ATTRIBUTI PRIVATE:
 *   - std::vector<evento*> eventiRaggruppati: lista puntatori agli eventi raggruppati
 * 
 * METODI:
 *   - raggruppa(...): costruttore
 *   - stampaEventiRaggruppati(): stampa lista eventi
 *   - stampa(): stampa sommario (override)
 *   - aggiungiEvento(): aggiunge evento al raggruppamento
 *   - rimuoviEvento(): rimuove evento dal raggruppamento
 *   - numEventi(): restituisce numero eventi
 *   - getEvento(): restituisce evento per indice
 */
class raggruppa: public eventoLungo
{
    private:
        std::vector<evento*> eventiRaggruppati;

    public:
        // Costruttore: inizializza raggruppamento di eventi
        // Input: int id, dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione
        // Output: oggetto raggruppa inizializzato
        raggruppa(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione);

        // Stampa a console la lista di tutti gli eventi raggruppati
        // Input: nessuno
        // Output: stampa numero e dettagli di ogni evento
        void stampaEventiRaggruppati();

        // Stampa il sommario del raggruppamento
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione e numero di eventi raggruppati
        void stampa() const override;

        // Aggiunge un nuovo evento al raggruppamento
        // Input: evento* nuovoEvento (non null)
        // Output: aggiunge evento al vettore se valido
        void aggiungiEvento(evento* nuovoEvento);

        // Rimuove un evento dal raggruppamento
        // Input: evento* eventoDaRimuovere (non null)
        // Output: rimuove evento se trovato
        void rimuoviEvento(evento* eventoDaRimuovere);

        // Getter: restituisce il numero di eventi raggruppati
        // Input: nessuno
        // Output: int numero di eventi
        int numEventi() const;

        // Getter: restituisce l'evento all'indice specificato
        // Input: int index (0-based)
        // Output: evento* (nullptr se indice non valido)
        evento* getEvento(int index) const;
};

/*
 * CLASSE: festivita
 * Descrizione: Rappresenta una festivita italiana predefinita
 * Estende: eventoLungo
 * 
 * ATTRIBUTI PRIVATE:
 *   - static const int id: identificativo statico
 *   - std::vector<std::string> festeItaliane: lista festivita predefinite
 * 
 * METODI:
 *   - festivita(...): costruttore
 *   - createFeste(): crea vector di festivita italiane predefinite
 *   - stampa(): stampa dettagli festivita (override)
 */
class festivita: public eventoLungo
{
    private:
        static const int id;
        std::vector<std::string> festeItaliane;

    public:
        // Costruttore: inizializza una festivita
        // Input: dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione, orario inizio, orario fine
        // Output: oggetto festivita inizializzato
        festivita(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine);

        // Crea automaticamente vector di festivita italiane predefinite (Natale, Pasqua, ecc.)
        // Input: nessuno
        // Output: std::vector<eventoLungo*> contiene 9 festivita italiane
        std::vector<eventoLungo*> createFeste();

        // Stampa i dettagli della festivita e lista festivita disponibili
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione, Priorita e lista festivita italiane
        void stampa() const override;
};

/*
 * CLASSE: compleanno
 * Descrizione: Rappresenta un evento di compleanno con lista partecipanti
 * Estende: eventoLungo
 * 
 * ATTRIBUTI PRIVATE:
 *   - static const int id: identificativo statico
 *   - int partecipanti: numero di partecipanti
 *   - std::vector<std::string> NomePartecipanti: lista nomi partecipanti
 * 
 * METODI:
 *   - compleanno(...): costruttore
 *   - aggiungiPartecipante(): aggiunge nome partecipante
 *   - stampa(): stampa dettagli compleanno (override)
 */
class compleanno: public eventoLungo
{
    private:
        static const int id;
        int partecipanti;
        std::vector<std::string> NomePartecipanti;

    public:
        // Costruttore: inizializza compleanno con numero iniziale partecipanti
        // Input: dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione, int partecipanti, orario inizio, orario fine
        // Output: oggetto compleanno inizializzato
        compleanno(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, int partecipanti, orario inizio, orario fine);

        // Aggiunge nome partecipante alla lista (se non vuoto)
        // Input: std::string nome
        // Output: incrementa contatore e aggiunge nome al vettore
        void aggiungiPartecipante(std::string nome);

        // Stampa dettagli del compleanno e lista partecipanti
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione, numero e lista partecipanti
        void stampa() const override;
};

/*
 * CLASSE: riunione
 * Descrizione: Rappresenta una riunione con contatti email e telefono dei partecipanti
 * Estende: eventoLungo
 * 
 * ATTRIBUTI PRIVATE:
 *   - static const int id: identificativo statico
 *   - std::vector<std::string> mailPartecipanti: lista email partecipanti
 *   - std::vector<int> TelPartecipanti: lista telefoni partecipanti
 * 
 * METODI:
 *   - riunione(...): costruttore
 *   - aggiungiPartecipante(): aggiunge email e telefono partecipante
 *   - stampa(): stampa dettagli riunione (override)
 */
class riunione: public eventoLungo
{
    private:
        static const int id;
        std::vector<std::string> mailPartecipanti;
        std::vector<int> TelPartecipanti;

    public:
        // Costruttore: inizializza riunione con liste partecipanti
        // Input: dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione, vector<string> mailPartecipanti, vector<int> TelPartecipanti
        // Output: oggetto riunione inizializzato
        riunione(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::vector<std::string> mailPartecipanti, std::vector<int> TelPartecipanti);

        // Aggiunge email e telefono di un partecipante (se validi)
        // Input: std::string mail (non vuota), int tel (positivo)
        // Output: aggiunge a vettori se dati validi
        void aggiungiPartecipante(std::string mail, int tel);

        // Stampa dettagli della riunione e lista partecipanti con contatti
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione, numero e lista (email|telefono) partecipanti
        void stampa() const override;
};

/*
 * CLASSE: altroTipo
 * Descrizione: Rappresenta un evento lungo di tipo generico con una particolarita
 * Estende: eventoLungo
 * 
 * ATTRIBUTI PRIVATE:
 *   - static const int id: identificativo statico
 *   - std::string Particolarita: caratteristica speciale dell'evento
 * 
 * METODI:
 *   - altroTipo(...): costruttore
 *   - stampa(): stampa dettagli evento (override)
 *   - getParticolarita(): restituisce particolarita
 *   - modificaParticolarita(): modifica particolarita
 */
class altroTipo: public eventoLungo
{
    private:
        static const int id;
        std::string Particolarita;

    public:
        // Costruttore: inizializza evento generico con particolarita
        // Input: dateTime momentoInizio, unsigned int priorita, string nome, dateTime momentoFine, string Descrizione, string Particolarita
        // Output: oggetto altroTipo inizializzato
        altroTipo(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::string Particolarita);

        // Stampa dettagli dell'evento generico
        // Input: nessuno
        // Output: stampa ID, Nome, Descrizione, Priorita e Particolarita
        void stampa() const override;

        // Getter: restituisce la particolarita dell'evento
        // Input: nessuno
        // Output: std::string Particolarita
        std::string getParticolarita() const;

        // Modifica la particolarita dell'evento (se non vuota)
        // Input: std::string nuovaParticolarita
        // Output: aggiorna Particolarita se non vuota
        void modificaParticolarita(std::string nuovaParticolarita);
};

// ==================== CLASSE CATEGORIA ====================
/*
 * CLASSE: categoriaPersona
 * Descrizione: Raggruppa impegni per categoria creata dall'utente
 * 
 * ATTRIBUTI PRIVATE:
 *   - raggruppa eventiCategoria: contenitore degli eventi della categoria
 * 
 * METODI:
 *   - creaCategoriaPersona(): aggiunge più eventi alla categoria
 *   - eliminaCategoriaPersona(): rimuove evento dalla categoria
 *   - stampaCategoriaPersona(): stampa eventi della categoria
 *   - modificaCategoriaPersona(): sostituisce evento con uno nuovo
 */
class categoriaPersona
{
    private:
        raggruppa eventiCategoria;

    public:
        // Crea categoria e aggiunge multiple eventi a essa
        // Input: std::vector<evento*> eventi, int numEventi
        // Output: aggiunge gli eventi validi (non null) al raggruppamento
        void creaCategoriaPersona(std::vector<evento*> eventi, int numEventi);

        // Rimuove un evento dalla categoria tramite ID
        // Input: evento* eventoR (evento da cercare per ID)
        // Output: rimuove evento se trovato nella categoria
        void eliminaCategoriaPersona(evento* eventoR);

        // Stampa a console tutti gli eventi della categoria
        // Input: nessuno
        // Output: stampa lista di tutti gli eventi raggruppati
        void stampaCategoriaPersona();

        // Sostituisce un evento con uno nuovo mantenendo la categoria
        // Input: evento* eventoR (evento attuale da trovare), evento* nuovoEvento (nuovo evento)
        // Output: rimuove eventoR e aggiunge nuovoEvento se match trovato per ID
        void modificaCategoriaPersona(evento* eventoR, evento* nuovoEvento);
};