#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

// include personali
#include <dataora.h>
#include <persona.h>


/*
    File header gerarchia persona
        Prima stesura: 2 / 2 / 2026
    
    Controllare:
        correttezza campi private e campi public.
        Vogliamo garantire information hiding?
    Finire:
        costruttori
*/

// primo livello
class impegno
{
    private:
        dateTime momentoInizio;
        const int id;
        uint8_t priorita;
    public:
        virtual ~impegno() =0;
};

// secondo livello
class evento: public impegno
{
    private:
        std::string nome;
    public:
        ~evento();
};

// terzo livello
class promemoria: public evento
{
    private:
        std::string descrizione;
    public:
        void modificaPromemoria();
};

class eventoLungo: public evento
{
    private:
        dateTime momentoFine;
        std::string Descrizione;
    public:
        virtual ~eventoLungo() =0;
        virtual void modificaEventoLungo();
};

// quarto livello
class raggruppa: public eventoLungo
{
    private:
        std::vector<evento*> eventiRaggruppati;
    public:
        void stampaEventiRaggruppati();
};

class festivita: public eventoLungo
{
    private:
        static const uint8_t id;
};

class compleanno: public eventoLungo
{
    private:
        static const uint8_t id;
};

class riunione: public eventoLungo
{
    private:
        static const uint8_t id;
};

class altroTipo: public eventoLungo
{
    private:
        static const uint8_t id;
};

// a parte: categoria creata dall'utente
class categoriaPersona
{
    private:
        // impegno
    public:
        void creaCategoriaPersona();
        void eliminaCategoriaPersona(); 
        void stampaCategoriaPersona();
        void modificaCategoriaPersona();
};