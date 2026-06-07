// test_io.cpp
// Test semplici per import/export JSON senza dipendenze esterne.
// Compilare con: g++ -std=c++17 JsonManager.cpp test_io.cpp -o tests
// Eseguire: ./tests

#include "JsonManager.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>

namespace fs = std::filesystem;

static bool evento_equal(const Evento& a, const Evento& b) {
    // confronta i campi principali (momenti confrontati come stringhe ISO)
    auto toStr = [](const dateTime& dt){
        std::time_t tt = std::chrono::system_clock::to_time_t(dt);
        std::tm tm;
    #if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &tt);
    #else
        localtime_r(&tt, &tm);
    #endif
        std::ostringstream out;
        out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
        return out.str();
    };
    auto optToStr = [&](const std::optional<dateTime>& dt) {
        return dt.has_value() ? toStr(*dt) : std::string();
    };
    if (a.id != b.id) return false;
    if (toStr(a.momentoInizio) != toStr(b.momentoInizio)) return false;
    if (optToStr(a.momentoFine) != optToStr(b.momentoFine)) return false;
    if (a.priorita != b.priorita) return false;
    if (a.nome != b.nome) return false;
    if (a.inizio.has_value() != b.inizio.has_value()) return false;
    if (a.inizio.has_value() && (*a.inizio != *b.inizio)) return false;
    if (a.fine.has_value() != b.fine.has_value()) return false;
    if (a.fine.has_value() && (*a.fine != *b.fine)) return false;
    if (a.Descrizione.has_value() != b.Descrizione.has_value()) return false;
    if (a.Descrizione.has_value() && (*a.Descrizione != *b.Descrizione)) return false;
    if (a.Particolarita.has_value() != b.Particolarita.has_value()) return false;
    if (a.Particolarita.has_value() && (*a.Particolarita != *b.Particolarita)) return false;
    return true;
}

int main() {
    int failures = 0;

    // File temporanei
    const std::string f1 = "test_eventi_io.json";
    const std::string f2 = "test_eventi_invalid.json";

    // 1) Test export/import roundtrip con campi opzionali null e presenti
    {
        Evento a;
        a.id = 1;
        a.momentoInizio = std::chrono::system_clock::now();
        a.momentoFine = a.momentoInizio + std::chrono::minutes(60);
        a.priorita = 2;
        a.nome = "Standup";
        a.inizio = std::string("09:00:00");
        a.fine = std::string("10:00:00");
        a.Descrizione = std::nullopt;
        a.Particolarita = std::nullopt;

        Evento b = a;
        b.id = 2;
        b.momentoInizio = *a.momentoFine + std::chrono::minutes(10);
        b.momentoFine = b.momentoInizio + std::chrono::minutes(30);
        b.priorita = 1;
        b.nome = "Revisione";
        b.inizio = std::string("10:10:00");
        b.fine = std::string("10:40:00");
        b.Descrizione = std::string("Revisione PR");
        b.Particolarita = std::string("Portare laptop");

        std::vector<Evento> lista = { a, b };
        std::cout << "DEBUG JSON EXPORT:\n" << JsonManager::toJsonString(lista) << "\n";

        std::string err;
        if (!JsonManager::exportToFile(f1, lista, err)) {
            std::cerr << "EXPORT FAILED: " << err << std::endl;
            ++failures;
        } else {
            std::vector<Evento> imported = JsonManager::importFromFile(f1, err);
            if (!err.empty()) {
                std::cerr << "IMPORT FAILED: " << err << std::endl;
                ++failures;
            } else {
                if (imported.size() != lista.size()) {
                    std::cerr << "IMPORT SIZE MISMATCH\n";
                    ++failures;
                } else {
                    for (size_t i = 0; i < lista.size(); ++i) {
                        std::cout << "DEBUG IMPORT INDEX " << i << "\n";
                        std::cout << " expected id=" << lista[i].id << " nome=" << lista[i].nome << " priorita=" << lista[i].priorita << "\n";
                        std::cout << " imported id=" << imported[i].id << " nome=" << imported[i].nome << " priorita=" << imported[i].priorita << "\n";
                        std::cout << " expected inizio=" << (lista[i].inizio.has_value() ? *lista[i].inizio : "null") << " fine=" << (lista[i].fine.has_value() ? *lista[i].fine : "null") << "\n";
                        std::cout << " imported inizio=" << (imported[i].inizio.has_value() ? *imported[i].inizio : "null") << " fine=" << (imported[i].fine.has_value() ? *imported[i].fine : "null") << "\n";
                        std::cout << " expected momentoFine=" << (lista[i].momentoFine.has_value() ? std::to_string(std::chrono::duration_cast<std::chrono::seconds>(lista[i].momentoFine->time_since_epoch()).count()) : std::string("null")) << "\n";
                        std::cout << " imported momentoFine=" << (imported[i].momentoFine.has_value() ? std::to_string(std::chrono::duration_cast<std::chrono::seconds>(imported[i].momentoFine->time_since_epoch()).count()) : std::string("null")) << "\n";
                        std::cout << " expected desc=" << (lista[i].Descrizione.has_value() ? *lista[i].Descrizione : "null") << " part=" << (lista[i].Particolarita.has_value() ? *lista[i].Particolarita : "null") << "\n";
                        std::cout << " imported desc=" << (imported[i].Descrizione.has_value() ? *imported[i].Descrizione : "null") << " part=" << (imported[i].Particolarita.has_value() ? *imported[i].Particolarita : "null") << "\n";
                        if (!evento_equal(lista[i], imported[i])) {
                            std::cerr << "EVENT MISMATCH AT INDEX " << i << "\n";
                            ++failures;
                        }
                    }
                }
            }
        }
    }

    // 2) Test import di JSON con tipo errato (priorita come stringa) -> deve restituire errore
    {
        // costruiamo manualmente un JSON non valido
        std::ofstream ofs(f2);
        ofs << R"([
  {
    "id": 1,
    "momentoInizio": "2026-06-07T10:00:00",
    "priorita": "alto",
    "nome": "EventoErrato",
    "momentoFine": "2026-06-07T11:00:00",
    "Descrizione": null,
    "Particolarita": null
  }
])";
        ofs.close();

        std::string err;
        auto imported = JsonManager::importFromFile(f2, err);
        if (err.empty()) {
            std::cerr << "ERROR: import di JSON con tipo errato non ha fallito come atteso\n";
            ++failures;
        } else {
            std::cout << "Import error correttamente rilevato: " << err << "\n";
        }
    }

    // cleanup
    try {
        if (fs::exists(f1)) fs::remove(f1);
        if (fs::exists(f2)) fs::remove(f2);
    } catch (...) {
        // non fatale
    }

    if (failures == 0) {
        std::cout << "TUTTI I TEST PASSATI\n";
        return 0;
    } else {
        std::cerr << failures << " TEST FALLITI\n";
        return 1;
    }
}
