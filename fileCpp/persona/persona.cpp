#include <string>
#include <cstdint>
#include <dataora.h>

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
        const int id;
        bool policy;
    public:
        utente();
        virtual ~utente() =0;
        virtual void gestionePolicy() =0;
};

class persona: public utente
{
    private:
        std::string email;
        std::string password;
        std::string nome;
        dateTime dataNascita;
    public:
        persona(std::string em = "",std::string p = "", std::string n = "", std::string dt = "" ): email(em), password(p), nome(n), dataNascita(dt != "" ? dt : nullptr){};
        std::string getEmail() const{
            return email;
        };
        std::string getNome() const{
            return nome;
        };
        std::string getDataNascita() const{
            dataNascita.getDate();
        };
        // passare come parametri bit 0 1 per la modifica del campi dati?
        // esempio: modicaPersona(int bitModEmail =0, int bitModPassword =0, int bitModNome =0, int bitModDataNascita =0)
        // default = 0 perché potrei non voler modificare tutti i campi nello stesso momento
        void modificaPersona(std::string em = "",std::string p = "", std::string n = "", std::string dt = "" ){
            if(em != "") email = em;
            if(p != "") password = p;
            if(n != "") nome = n;
            if(dt != "") dataNascita = dt;

        };

        void gestionePolicy();
        
};