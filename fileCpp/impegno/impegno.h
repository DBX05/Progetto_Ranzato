#ifndef IMPEGNO_H
#define IMPEGNO_H

#pragma once

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

#include "../dataora/dataora.h"
#include "../persona/persona.h"

// ==================== PRIMO LIVELLO ====================
class impegno
{
private:
    dateTime momentoInizio;
    const int id;
    unsigned int priorita;

public:
    impegno(int id, dateTime momentoInizio, unsigned int priorita);
    virtual ~impegno() = 0;

    virtual dateTime getMomentoInizio() const;
    virtual unsigned int getPriorita() const;
    virtual int getId() const;
    virtual void modificaImpegno(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita);
    virtual void stampa() const = 0;
};

// ==================== SECONDO LIVELLO ====================
class evento: public impegno
{
private:
    orario inizio;
    orario fine;
    std::string nome;

public:
    evento(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine);
    std::string getNome() const;
    std::string getInizio() const;
    std::string getFine() const;
    void modificaName(std::string nuovoNome);
    void stampa() const override;
    ~evento();
};

// ==================== TERZO LIVELLO ====================
class promemoria: public evento
{
private:
    std::string descrizione;

public:
    promemoria(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, std::string descrizione, orario inizio, orario fine);
    std::string getDescrizione() const;
    void modificaPromemoria(std::string nuovaDescrizione);
    void stampa() const override;
};

// ==================== eventoLungo ====================
class eventoLungo: public evento
{
private:
    dateTime momentoFine;
    std::string Descrizione;

public:
    eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine);
    eventoLungo(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, orario inizio, orario fine);

    dateTime getMomentoFine() const;
    std::string getDescrizione() const;
    virtual int getType() const;
    virtual ~eventoLungo();

    virtual void modificaEventoLungo(dateTime nuovoMomentoInizio, unsigned int nuovaPriorita, dateTime nuovoMomentoFine, std::string nuovaDescrizione);
    virtual void stampa() const;
};

// ==================== QUARTO LIVELLO ====================
class raggruppa: public eventoLungo
{
private:
    std::vector<evento*> eventiRaggruppati;

public:
    raggruppa(int id, dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione);
    int getType() const override;
    void stampaEventiRaggruppati();
    void stampa() const override;
    void aggiungiEvento(evento* nuovoEvento);
    void rimuoviEvento(evento* eventoDaRimuovere);
    int numEventi() const;
    evento* getEvento(int index) const;
};

class festivita: public eventoLungo
{
private:
    static const int id;
    static std::vector<std::string> festeItaliane;

public:
    festivita(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, orario inizio, orario fine);
    std::vector<eventoLungo*> createFeste();
    void stampa() const override;
    int getType() const override;
};

class compleanno: public eventoLungo
{
private:
    static const int id;
    int partecipanti;
    std::vector<std::string> NomePartecipanti;

public:
    compleanno(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, int partecipanti, orario inizio, orario fine);
    int getType() const override;
    void aggiungiPartecipante(std::string nome);
    void stampa() const override;
};

class riunione: public eventoLungo
{
private:
    static const int id;
    std::vector<std::string> mailPartecipanti;
    std::vector<int> TelPartecipanti;

public:
    riunione(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::vector<std::string> mailPartecipanti, std::vector<int> TelPartecipanti);
    int getType() const override;
    void aggiungiPartecipante(std::string mail, int tel);
    void stampa() const override;
};

class altroTipo: public eventoLungo
{
private:
    static const int id;
    std::string Particolarita;

public:
    altroTipo(dateTime momentoInizio, unsigned int priorita, std::string nome, dateTime momentoFine, std::string Descrizione, std::string Particolarita);
    void stampa() const override;
    int getType() const override;
    std::string getParticolarita() const;
    void modificaParticolarita(std::string nuovaParticolarita);
};

// ==================== CATEGORIA ====================
class categoriaPersona
{
private:
    raggruppa eventiCategoria;

public:
    void creaCategoriaPersona(std::vector<evento*> eventi, int numEventi);
    void eliminaCategoriaPersona(evento* eventoR);
    void stampaCategoriaPersona();
    void modificaCategoriaPersona(evento* eventoR, evento* nuovoEvento);
};

#endif // IMPEGNO_H
