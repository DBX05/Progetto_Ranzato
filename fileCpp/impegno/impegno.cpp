#include "impegno.h"
#include "../dataora/dataora.h"
#include <iostream>
#include <utility>

// Distruttore puro virtuale definito
impegno::~impegno() = default;

// Costruttore impegno
impegno::impegno(int id, dateTime momentoInizio, unsigned int priorita)
    : momentoInizio(momentoInizio), id(id < 0 ? 0 : id), priorita(priorita) {}

// Getter e metodi base
dateTime impegno::getMomentoInizio() const { return momentoInizio; }
unsigned int impegno::getPriorita() const { return priorita; }
int impegno::getId() const { return id; }
void impegno::modificaImpegno(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita){
    momentoInizio = nuovoMomentoInizio;
    priorita = nuovaPriorita;
}

// ==================== evento ====================
evento::evento(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine)
    : impegno(id, momentoInizio, priorita),
      inizio(inizio),
      fine(fine),
      nome(nome.empty() ? "Evento senza nome" : std::move(nome))
{
}

std::string evento::getNome() const { return nome; }
std::string evento::getInizio() const { return inizio.curTime(); }
std::string evento::getFine() const { return fine.curTime(); }
void evento::modificaName(std::string nuovoNome) { if (!nuovoNome.empty()) nome = std::move(nuovoNome); }
void evento::stampa() const {
    std::cout << "=== Evento ===\nID: " << getId() << "\nNome: " << nome
              << "\nInizio: " << getInizio() << "\nFine: " << getFine()
              << "\nPriorita: " << getPriorita() << std::endl;
}
evento::~evento() = default;

// ==================== eventoLungo ====================
eventoLungo::eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine)
    : evento(id, momentoInizio, priorita, std::move(nome), inizio, fine),
      momentoFine(momentoFine),
      Descrizione(std::move(Descrizione))
{
}

eventoLungo::eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine)
    : evento(id, momentoInizio, priorita, std::move(nome), inizio, fine),
      momentoFine(momentoInizio),
      Descrizione("Evento lungo senza descrizione")
{
}

dateTime eventoLungo::getMomentoFine() const { return momentoFine; }
std::string eventoLungo::getDescrizione() const { return Descrizione; }
int eventoLungo::getType() const { return 0; }
eventoLungo::~eventoLungo() = default;

void eventoLungo::modificaEventoLungo(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita, dateTime nuovoMomentoFine, std::string nuovaDescrizione) {
    modificaImpegno(nuovoMomentoInizio, nuovaPriorita);
    if (!(nuovoMomentoFine < nuovoMomentoInizio)) momentoFine = nuovoMomentoFine;
    if (!nuovaDescrizione.empty()) Descrizione = std::move(nuovaDescrizione);
}
void eventoLungo::stampa() const {
    std::cout << "=== Evento Lungo ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nInizio: " << getInizio()
              << "\nFine: " << getMomentoFine().getDateTime() << "\nPriorita: " << getPriorita() << std::endl;
}

// ==================== raggruppa ====================
raggruppa::raggruppa(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione)
    : eventoLungo(id, momentoInizio, priorita, std::move(nome), momentoFine, std::move(Descrizione), orario(0,0,0), orario(23,59))
{
}

int raggruppa::getType() const { return 3; }

void raggruppa::stampaEventiRaggruppati() {
    for (int i = 0; i < static_cast<int>(eventiRaggruppati.size()); ++i) {
        if (eventiRaggruppati[i]) {
            std::cout << "Evento " << i + 1 << ": " << eventiRaggruppati[i]->getNome()
                      << " - " << eventiRaggruppati[i]->getInizio()
                      << " - " << eventiRaggruppati[i]->getFine() << std::endl;
        }
    }
}

void raggruppa::stampa() const {
    std::cout << "=== Raggruppa ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nEventi raggruppati: " << numEventi() << std::endl;
}

void raggruppa::aggiungiEvento(evento* nuovoEvento){
    if (nuovoEvento == nullptr) return;
    eventiRaggruppati.push_back(nuovoEvento);
}

void raggruppa::rimuoviEvento(evento* eventoDaRimuovere){
    if (eventoDaRimuovere == nullptr) return;
    for (int i = 0; i < static_cast<int>(eventiRaggruppati.size()); ++i) {
        if (eventiRaggruppati[i] == eventoDaRimuovere) {
            eventiRaggruppati.erase(eventiRaggruppati.begin() + i);
            break;
        }
    }
}

int raggruppa::numEventi() const { return static_cast<int>(eventiRaggruppati.size()); }

evento* raggruppa::getEvento(int index) const {
    if (index < 0 || index >= static_cast<int>(eventiRaggruppati.size())) return nullptr;
    return eventiRaggruppati[index];
}

// ==================== festivita ====================
festivita::festivita(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine)
    : eventoLungo(festivita::id, momentoInizio, priorita, std::move(nome), momentoFine, std::move(Descrizione), inizio, fine)
{
}


// definizione del vettore statico festeItaliane
std::vector<std::string> festivita::festeItaliane = {
    "Natale", "Pasqua", "Capodanno", "Ferragosto", "Ognissanti",
    "Epifania", "San Valentino", "Festa della mamma", "Festa del papà"
};


std::vector<eventoLungo*> festivita::createFeste(){
    std::vector<eventoLungo*> festivitaItaliane;
    for (int i = 0; i < static_cast<int>(festeItaliane.size()); ++i) {
        festivita* festa = nullptr;
        switch(i){
            case 0:
                festa = new festivita(dateTime(0, 12, 25), 0, "Natale", dateTime(0, 12, 25), "Festa di Natale", orario(0, 0), orario(23, 59));
                break;
            case 1:
                festa = new festivita(dateTime(0, 4, 4), 0, "Pasqua", dateTime(0, 4, 4), "Festa di Pasqua", orario(0, 0), orario(23, 59));
                break;
            case 2:
                festa = new festivita(dateTime(0, 1, 1), 0, "Capodanno", dateTime(0, 1, 1), "Festa di Capodanno", orario(0, 0), orario(23, 59));
                break;
            case 3:
                festa = new festivita(dateTime(0, 8, 15), 0, "Ferragosto", dateTime(0, 8, 15), "Festa di Ferragosto", orario(0, 0), orario(23, 59));
                break;
            case 4:
                festa = new festivita(dateTime(0, 11, 1), 0, "Ognissanti", dateTime(0, 11, 1), "Festa di Ognissanti", orario(0, 0), orario(23, 59));
                break;
            case 5:
                festa = new festivita(dateTime(0, 1, 6), 0, "Epifania", dateTime(0, 1, 6), "Festa di Epifania", orario(0, 0), orario(23, 59));
                break;
            case 6:
                festa = new festivita(dateTime(0, 2, 14), 0, "San Valentino", dateTime(0, 2, 14), "Festa di San Valentino", orario(0, 0), orario(23, 59));
                break;
            case 7:
                festa = new festivita(dateTime(0, 3, 8), 0, "Festa della mamma", dateTime(0, 3, 8), "Festa della mamma", orario(0, 0), orario(23, 59));
                break;
            case 8:
                festa = new festivita(dateTime(0, 3, 19), 0, "Festa del papà", dateTime(0, 3, 19), "Festa del papà", orario(0, 0), orario(23, 59));
                break;
            default:
                break;
        }
        if (festa) festivitaItaliane.push_back(festa);
    }
    return festivitaItaliane;
}
// implementazione mancante: getType per festivita
int festivita::getType() const {
    return 0; // oppure il codice che preferisci per il tipo (es. 0 = generico)
}

// Se non l'hai già definita, assicurati che la firma corrisponda a quella in header:
// void festivita::stampa() const { ... }  // già presente altrove nel file


void festivita::stampa() const {
    std::cout << "=== Festivita ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nFeste italiane disponibili: " << festeItaliane.size() << std::endl;
}

// ==================== compleanno ====================
compleanno::compleanno(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, int partecipanti, orario inizio, orario fine)
    : eventoLungo(compleanno::id, momentoInizio, priorita, std::move(nome), momentoFine, std::move(Descrizione), inizio, fine),
      partecipanti(partecipanti < 0 ? 0 : partecipanti)
{
}

int compleanno::getType() const { return 1; }

void compleanno::aggiungiPartecipante(std::string nome){
    if (nome.empty()) return;
    NomePartecipanti.push_back(std::move(nome));
    ++partecipanti;
}

void compleanno::stampa() const {
    std::cout << "=== Compleanno ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nPartecipanti: " << partecipanti << std::endl;
    for (const auto &n : NomePartecipanti) std::cout << "  - " << n << std::endl;
}

// ==================== riunione ====================
riunione::riunione(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::vector<std::string> mailPartecipanti, std::vector<int> TelPartecipanti)
    : eventoLungo(riunione::id, momentoInizio, priorita, std::move(nome), momentoFine, std::move(Descrizione), orario(0,0,0), orario(23,59)),
      mailPartecipanti(std::move(mailPartecipanti)), TelPartecipanti(std::move(TelPartecipanti))
{
}

int riunione::getType() const { return 2; }

void riunione::aggiungiPartecipante(std::string mail, int tel){
    if (mail.empty() || tel <= 0) return;
    mailPartecipanti.push_back(std::move(mail));
    TelPartecipanti.push_back(tel);
}

void riunione::stampa() const {
    std::cout << "=== Riunione ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nPartecipanti (" << mailPartecipanti.size() << "):" << std::endl;
    for (size_t i = 0; i < mailPartecipanti.size(); ++i) {
        std::cout << "  - Mail: " << mailPartecipanti[i] << " | Tel: " << TelPartecipanti[i] << std::endl;
    }
}

// ==================== altroTipo ====================
altroTipo::altroTipo(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::string Particolarita)
    : eventoLungo(altroTipo::id, momentoInizio, priorita, std::move(nome), momentoFine, std::move(Descrizione), orario(0,0,0), orario(23,59)),
      Particolarita(std::move(Particolarita))
{
}

int altroTipo::getType() const { return 4; }

void altroTipo::stampa() const {
    std::cout << "=== Altro Tipo Evento ===\nID: " << getId() << "\nNome: " << getNome()
              << "\nDescrizione: " << getDescrizione() << "\nPriorita: " << getPriorita()
              << "\nParticolarita: " << Particolarita << std::endl;
}

std::string altroTipo::getParticolarita() const { return Particolarita; }
void altroTipo::modificaParticolarita(std::string nuovaParticolarita){
    if(nuovaParticolarita.empty()) return;
    Particolarita = std::move(nuovaParticolarita);
}

// ==================== categoriaPersona ====================
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

// ==================== definizioni statiche e membri festivi ====================
// assegna valori univoci agli id statici (una sola definizione per ciascuno)
const int festivita::id = 100;
const int compleanno::id = 101;
const int riunione::id = 102;
const int altroTipo::id = 103;
