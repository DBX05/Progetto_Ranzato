// JsonManager.hpp
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>

using dateTime = std::chrono::system_clock::time_point;

struct Evento {
    int id;
    dateTime momentoInizio;
    unsigned int priorita;
    std::string nome;
    std::optional<std::string> inizio;        // orario dell'evento
    std::optional<std::string> fine;          // orario dell'evento
    std::optional<dateTime> momentoFine;      // per eventi lunghi
    std::optional<std::string> Descrizione;   // opzionale, può essere null
    std::optional<std::string> Particolarita; // opzionale, può essere null
};

class JsonManager {
public:
    // Importa da file JSON; in caso di errore 'error' contiene il messaggio e il vettore restituito è vuoto.
    static std::vector<Evento> importFromFile(const std::string& filename, std::string& error);

    // Esporta vettore di Eventi in file JSON; i campi opzionali vengono inclusi e possono essere null.
    static bool exportToFile(const std::string& filename, const std::vector<Evento>& eventi, std::string& error);

    // Utility: converte vettore Eventi in stringa JSON (per test)
    static std::string toJsonString(const std::vector<Evento>& eventi);
};
