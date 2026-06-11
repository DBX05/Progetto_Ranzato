#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <impegno.h>

// include personali
#include <fileCpp/dataora/dataora.h>
#include <fileCpp/persona/persona.h>

// ==================== PRIMO LIVELLO ====================
// ==================== CLASSE IMPEGNO ====================
impegno::impegno(int id, dateTime momentoInizio, unsigned int priorita): id(id < 0 ? 0 : id), momentoInizio(momentoInizio), priorita(priorita < 0 ? 0 : priorita) {}
dateTime impegno::getMomentoInizio() const { return momentoInizio; }
unsigned int impegno::getPriorita() const { return priorita; }
int impegno::getId() const { return id; }
void impegno::modificaImpegno(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita){
    momentoInizio = nuovoMomentoInizio;
    priorita = nuovaPriorita;
}
// ==================== SECONDO LIVELLO ====================
// ==================== CLASSE EVENTO ====================
evento::evento(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine): impegno(id, momentoInizio, priorita), nome(nome.empty() ? "Evento senza nome" : nome), inizio(inizio == orario() ? orario(0, 0, 0) : inizio), fine(fine > inizio ? orario(0, 0, 0) : fine) {}
std::string evento::getNome() const { return nome; }
std::string evento::getInizio() const { return inizio.curTime(); }
std::string evento::getFine() const { return fine.curTime(); }
void evento::modificaName(std::string nuovoNome) {
    if(nuovoNome.empty())return;
    nome = nuovoNome;
}
void evento::stampa() const {
    std::cout << "=== Evento ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << nome << std::endl;
    std::cout << "Inizio: " << inizio.curTime() << std::endl;
    std::cout << "Fine: " << fine.curTime() << std::endl;
    std::cout << "Priorita: " << getPriorita() << std::endl;
}
// ==================== TERZO LIVELLO ====================
// ==================== CLASSE PROMEMORIA ====================
promemoria::promemoria(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, std::string descrizione,orario inizio, orario fine): evento(id, momentoInizio, priorita, nome,inizio,fine), descrizione(descrizione.empty() ? "Promemoria senza descrizione" : descrizione) {}
std::string promemoria::getDescrizione() const { return descrizione; }
void promemoria::modificaPromemoria(std::string nuovaDescrizione) {
    if(nuovaDescrizione.empty()) return ;
    descrizione = nuovaDescrizione;
}
void promemoria::stampa() const {
    std::cout << "=== Promemoria ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << descrizione << std::endl;
    std::cout << "Inizio: " << getInizio() << std::endl;
    std::cout << "Priorita: " << getPriorita() << std::endl;
}

// ==================== CLASSE EVENTOLUNGO ====================
eventoLungo::eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione,orario inizio, orario fine): evento(id, momentoInizio, priorita, nome,inizio,fine), momentoFine(momentoFine), Descrizione(Descrizione.empty() ? "Evento lungo senza descrizione" : Descrizione) {}
dateTime eventoLungo::getMomentoFine() const { return momentoFine; }
std::string eventoLungo::getDescrizione() const { return Descrizione; }
void eventoLungo::modificaEventoLungo(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita, dateTime nuovoMomentoFine, std::string nuovaDescrizione) {
    modificaImpegno(nuovoMomentoInizio, nuovaPriorita);
    if(!(nuovoMomentoFine < nuovoMomentoInizio)) momentoFine = nuovoMomentoFine;
    if(!nuovaDescrizione.empty()) Descrizione = nuovaDescrizione;;
}

// ==================== QUARTO LIVELLO ====================
// ==================== CLASSE FESTIVITA ====================
std::vector<std::string> festivita::festeItaliane = {"Natale", "Pasqua", "Capodanno", "Ferragosto", "Ognissanti", "Epifania", "San Valentino", "Festa della mamma", "Festa del papà"};
festivita::festivita(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione,orario inizio, orario fine): eventoLungo(id, momentoInizio, priorita, nome, momentoFine, Descrizione,inizio,fine) { }
std::vector<eventoLungo*> festivita::createFeste(){
    std::vector<eventoLungo*> festivitaItaliane;
    for(int i = 0; i < festeItaliane.size(); i++){
        festivita* festa = nullptr;
        switch(i){
            case 0:
                festa = new festivita(dateTime(0, 12, 25), 0, "Natale", dateTime(0, 12, 25), "Festa di Natale", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 1:
                festa = new festivita(dateTime(0, 4, 4), 0, "Pasqua", dateTime(0, 4, 4), "Festa di Pasqua", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 2:
                festa = new festivita(dateTime(0, 1, 1), 0, "Capodanno", dateTime(0, 1, 1), "Festa di Capodanno", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 3:
                festa = new festivita(dateTime(0, 8, 15), 0, "Ferragosto", dateTime(0, 8, 15), "Festa di Ferragosto", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 4:
                festa = new festivita(dateTime(0, 11, 1), 0, "Ognissanti", dateTime(0, 11, 1), "Festa di Ognissanti", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 5:
                festa = new festivita(dateTime(0, 1, 6), 0, "Epifania", dateTime(0, 1, 6), "Festa di Epifania", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 6:
                festa = new festivita(dateTime(0, 2, 14), 0, "San Valentino", dateTime(0, 2, 14), "Festa di San Valentino", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 7:
                festa = new festivita(dateTime(0, 3, 8), 0, "Festa della mamma", dateTime(0, 3, 8), "Festa della mamma", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            case 8:
                festa = new festivita(dateTime(0, 3, 19), 0, "Festa del papà", dateTime(0, 3, 19), "Festa del papà", orario(0, 0), orario(23, 59));
                festivitaItaliane.push_back(festa);
                break;
            default:
                break;
        }
    }
    return festivitaItaliane;
}
void festivita::stampa() const {
    std::cout << "=== Festivita ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << getDescrizione() << std::endl;
    std::cout << "Feste italiane disponibili: " << festeItaliane.size() << std::endl;
    for(int i = 0; i < festeItaliane.size(); i++){
        std::cout << "  - " << festeItaliane[i] << std::endl;
    }
}

// ==================== CLASSE COMPLEANNO ====================
compleanno::compleanno(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, int partecipanti,orario inizio, orario fine): eventoLungo(id, momentoInizio, priorita, nome, momentoFine, Descrizione,inizio,fine), partecipanti(partecipanti) {}
void compleanno::aggiungiPartecipante(std::string nome){
    if(nome.empty()) return;
    NomePartecipanti.push_back(nome);
    partecipanti++;
}
void compleanno::stampa() const {
    std::cout << "=== Compleanno ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << getDescrizione() << std::endl;
    std::cout << "Partecipanti: " << partecipanti << std::endl;
    for(int i = 0; i < NomePartecipanti.size(); i++){
        std::cout << "  - " << NomePartecipanti[i] << std::endl;
    }
}

// ==================== CLASSE RIUONIONE ====================
riunione::riunione(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::vector<std::string> mailPartecipanti, std::vector<int> TelPartecipanti): eventoLungo(id, momentoInizio, priorita, nome, momentoFine, Descrizione, orario(0, 0), orario(23, 59)), mailPartecipanti(mailPartecipanti), TelPartecipanti(TelPartecipanti) {}
void riunione::aggiungiPartecipante(std::string mail, int tel){
    if(mail.empty() || tel <= 0) return;
    mailPartecipanti.push_back(mail);
    TelPartecipanti.push_back(tel);
}
void riunione::stampa() const {
    std::cout << "=== Riunione ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << getDescrizione() << std::endl;
    std::cout << "Partecipanti (" << mailPartecipanti.size() << "):" << std::endl;
    for(int i = 0; i < mailPartecipanti.size(); i++){
        std::cout << "  - Mail: " << mailPartecipanti[i] << " | Tel: " << TelPartecipanti[i] << std::endl;
    }
}

// ==================== CLASSE ALTROTIPO ====================
altroTipo::altroTipo(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::string Particolarita): eventoLungo(id, momentoInizio, priorita, nome, momentoFine, Descrizione, orario(0, 0), orario(23, 59)), Particolarita(Particolarita) {}
void altroTipo::stampa() const {
    std::cout << "=== Altro Tipo Evento ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << getDescrizione() << std::endl;
    std::cout << "Priorita: " << getPriorita() << std::endl;
    std::cout << "Particolarita: " << Particolarita << std::endl;
}
std::string altroTipo::getParticolarita() const { return Particolarita; }
void altroTipo::modificaParticolarita(std::string nuovaParticolarita){
    if(nuovaParticolarita.empty()) return;
    Particolarita = nuovaParticolarita;
}
// ==================== CLASSE RAGGRUPPA ====================
raggruppa::raggruppa(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione): eventoLungo(id, momentoInizio, priorita, nome, momentoFine, Descrizione, orario(0, 0), orario(23, 59)) {}
void raggruppa::stampaEventiRaggruppati() {
    for(int i = 0; i < eventiRaggruppati.size(); i++){
        std::cout << "Evento " << i + 1 << ": " << eventiRaggruppati[i]->getNome() << " - " << eventiRaggruppati[i]->getInizio() << " - " << eventiRaggruppati[i]->getFine() << std::endl;
    }
}
void raggruppa::stampa() const {
    std::cout << "=== Raggruppa Evento ===" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Nome: " << getNome() << std::endl;
    std::cout << "Descrizione: " << getDescrizione() << std::endl;
    std::cout << "Eventi raggruppati: " << eventiRaggruppati.size() << std::endl;
}
void raggruppa::aggiungiEvento(evento* nuovoEvento){
    if(nuovoEvento == nullptr) return;
    eventiRaggruppati.push_back(nuovoEvento);
}
void raggruppa::rimuoviEvento(evento* eventoDaRimuovere){
    if(eventoDaRimuovere == nullptr) return;
    for(int i = 0; i < eventiRaggruppati.size(); i++){
        if(eventiRaggruppati[i] == eventoDaRimuovere){
            eventiRaggruppati.erase(eventiRaggruppati.begin() + i);
            break;
        }
    }
}
int raggruppa::numEventi() const { return eventiRaggruppati.size(); }
evento* raggruppa::getEvento(int index) const {
    if(index < 0 || index >= eventiRaggruppati.size()) return nullptr;
    return eventiRaggruppati[index];
}

// ==================== A PARTE ====================
// ==================== CLASSE CATEGORIAPERSONA ====================
void categoriaPersona::creaCategoriaPersona(std::vector<evento*> eventi, int numEventi){
    for(int i = 0; i < numEventi; i++){
        if(eventi[i] == nullptr) continue;
        eventiCategoria.aggiungiEvento(eventi[i]);
    }
}
void categoriaPersona::eliminaCategoriaPersona(evento* eventoR){
    for(int i = 0; i < eventiCategoria.numEventi(); i++){
        if(eventiCategoria.getEvento(i)->getId() == eventoR->getId()){
            eventiCategoria.rimuoviEvento(eventiCategoria.getEvento(i));
            break;
        }
    }
}
void categoriaPersona::stampaCategoriaPersona(){
    eventiCategoria.stampaEventiRaggruppati();
}
void categoriaPersona::modificaCategoriaPersona(evento* eventoR, evento* nuovoEvento){
    for(int i = 0; i < eventiCategoria.numEventi(); i++){
        if(eventiCategoria.getEvento(i)->getId() == eventoR->getId()){
            eventiCategoria.rimuoviEvento(eventoR);
            eventiCategoria.aggiungiEvento(nuovoEvento);
            break;
        }
    }
}