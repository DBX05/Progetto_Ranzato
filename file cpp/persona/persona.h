#include <string>
#include <cstdint>

/*
    File header gerarchia persona
        Prima stesura: 2 / 2 / 2026
    
    Controllare:
        correttezza campi private e campi public.
        Vogliamo garantire information hiding?
    Finire:
        costruttori
*/

class utente
{
    private:
        const uint32_t id;
        bool policy;
    public:
        utente();
        virtual ~utente() =0;
        void gestionePolicy();
};

class persona: public utente
{
    private:
        std::string email;
        const std::string password;
        std::string nome;
        std::string dataNascita;
    public:
        persona();
        std::string getEmail() const;
        std::string getPassowrd() const;
        std::string getNome() const;
        std::string getDataNascita() const;
        // passare come parametri bit 0 1 per la modifica del campi dati?
        // esempio: modicaPersona(int bitModEmail =0, int bitModPassword =0, int bitModNome =0, int bitModDataNascita =0)
        // default = 0 perché potrei non voler modificare tutti i campi nello stesso momento
        void modificaPersona();
        
};