// JsonManager.cpp
// Compilare con: g++ -std=c++17 JsonManager.cpp test_io.cpp -o tests
// Richiede nlohmann/json.hpp

#include "jsonmanager.hpp"
#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

using json = nlohmann::json;

// --- helper ISO8601 (YYYY-MM-DDTHH:MM:SS) <-> dateTime ---
static std::optional<dateTime> parseISO8601(const std::string& s) {
    std::istringstream in(s);
    std::tm tm = {};
    in >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (in.fail()) return std::nullopt;
    std::time_t tt = std::mktime(&tm);
    if (tt == -1) return std::nullopt;
    return std::chrono::system_clock::from_time_t(tt);
}

static std::string formatISO8601(const dateTime& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return out.str();
}

// --- Implementazione JsonManager ---
std::vector<Evento> JsonManager::importFromFile(const std::string& filename, std::string& error) {
    std::vector<Evento> result;
    error.clear();

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        error = "Impossibile aprire file: " + filename;
        return result;
    }

    json j;
    try {
        ifs >> j;
    } catch (const std::exception& e) {
        error = std::string("Errore parsing JSON: ") + e.what();
        return result;
    }

    if (!j.is_array()) {
        error = "Formato JSON non valido: atteso array di oggetti.";
        return result;
    }

    for (size_t idx = 0; idx < j.size(); ++idx) {
        const auto& item = j[idx];
        try {
            // Verifica campi obbligatori esistenti
            if (!item.contains("id") || !item.contains("momentoInizio") ||
                !item.contains("priorita") || !item.contains("nome")) {
                error = "Oggetto JSON all'indice " + std::to_string(idx) + " mancante di campi obbligatori.";
                return {};
            }

            if (!(item["id"].is_number_integer() || item["id"].is_number_unsigned())) {
                error = "Campo 'id' non è un numero intero nell'oggetto indice " + std::to_string(idx) + ".";
                return {};
            }

            // Tipo controllo: momentoInizio deve essere stringa
            if (!item["momentoInizio"].is_string()) {
                error = "Campo 'momentoInizio' non è una stringa nell'oggetto indice " + std::to_string(idx) + ".";
                return {};
            }

            // priorita deve essere numero intero (accettiamo signed/unsigned ma non negativo)
            if (!(item["priorita"].is_number_unsigned() || item["priorita"].is_number_integer())) {
                error = "Campo 'priorita' non è un numero intero nell'oggetto indice " + std::to_string(idx) + ".";
                return {};
            }

            if (!item["nome"].is_string()) {
                error = "Campo 'nome' non è una stringa nell'oggetto indice " + std::to_string(idx) + ".";
                return {};
            }

            Evento ev;
            ev.id = item["id"].get<int>();
            std::string sInizio = item.at("momentoInizio").get<std::string>();
            auto optIn = parseISO8601(sInizio);
            if (!optIn.has_value()) {
                error = "Formato data non valido (atteso ISO8601 YYYY-MM-DDTHH:MM:SS) Campo 'momentoInizio' nell'oggetto indice " + std::to_string(idx) + ".";
                return {};
            }
            ev.momentoInizio = *optIn;

            if (item.contains("momentoFine") && !item["momentoFine"].is_null()) {
                if (!item["momentoFine"].is_string()) {
                    error = "Campo 'momentoFine' non è una stringa nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
                std::string sFine = item.at("momentoFine").get<std::string>();
                auto optFi = parseISO8601(sFine);
                if (!optFi.has_value()) {
                    error = "Formato data non valido (atteso ISO8601 YYYY-MM-DDTHH:MM:SS) Campo 'momentoFine' nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
                ev.momentoFine = *optFi;
            } else {
                ev.momentoFine = std::nullopt;
            }

            if (item.contains("inizio")) {
                if (item["inizio"].is_null()) {
                    ev.inizio = std::nullopt;
                } else if (item["inizio"].is_string()) {
                    ev.inizio = item["inizio"].get<std::string>();
                } else {
                    error = "Campo 'inizio' presente ma non è stringa o null nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
            } else {
                ev.inizio = std::nullopt;
            }

            if (item.contains("fine")) {
                if (item["fine"].is_null()) {
                    ev.fine = std::nullopt;
                } else if (item["fine"].is_string()) {
                    ev.fine = item["fine"].get<std::string>();
                } else {
                    error = "Campo 'fine' presente ma non è stringa o null nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
            } else {
                ev.fine = std::nullopt;
            }

            // priorita: gestione valore negativo
            if (item["priorita"].is_number_unsigned()) {
                ev.priorita = item["priorita"].get<unsigned int>();
            } else {
                int tmp = item["priorita"].get<int>();
                if (tmp < 0) {
                    error = "Campo 'priorita' negativo nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
                ev.priorita = static_cast<unsigned int>(tmp);
            }

            ev.nome = item.at("nome").get<std::string>();

            // Campi opzionali: possono essere assenti, stringa o null. Altri tipi => errore.
            if (item.contains("Descrizione") || item.contains("descrizione")) {
                const auto& field = item.contains("Descrizione") ? item["Descrizione"] : item["descrizione"];
                if (field.is_null()) {
                    ev.Descrizione = std::nullopt;
                } else if (field.is_string()) {
                    ev.Descrizione = field.get<std::string>();
                } else {
                    error = "Campo 'Descrizione/descrizione' presente ma non è stringa o null nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
            } else {
                ev.Descrizione = std::nullopt;
            }

            if (item.contains("Particolarita")) {
                if (item["Particolarita"].is_null()) {
                    ev.Particolarita = std::nullopt;
                } else if (item["Particolarita"].is_string()) {
                    ev.Particolarita = item["Particolarita"].get<std::string>();
                } else {
                    error = "Campo 'Particolarita' presente ma non è stringa o null nell'oggetto indice " + std::to_string(idx) + ".";
                    return {};
                }
            } else {
                ev.Particolarita = std::nullopt;
            }

            result.push_back(std::move(ev));
        } catch (const std::exception& e) {
            error = std::string("Errore durante la lettura di un oggetto all'indice ") + std::to_string(idx) + ": " + e.what();
            return {};
        }
    }

    return result;
}

bool JsonManager::exportToFile(const std::string& filename, const std::vector<Evento>& eventi, std::string& error) {
    error.clear();
    json j = json::array();

    for (const auto& ev : eventi) {
        json item;
        item["id"] = ev.id;
        item["momentoInizio"] = formatISO8601(ev.momentoInizio);
        item["priorita"] = ev.priorita;
        item["nome"] = ev.nome;
        if (ev.inizio.has_value()) item["inizio"] = *ev.inizio;
        else item["inizio"] = nullptr;
        if (ev.fine.has_value()) item["fine"] = *ev.fine;
        else item["fine"] = nullptr;
        if (ev.momentoFine.has_value()) item["momentoFine"] = formatISO8601(*ev.momentoFine);
        else item["momentoFine"] = nullptr;
        if (ev.Descrizione.has_value()) item["Descrizione"] = *ev.Descrizione;
        else item["Descrizione"] = nullptr;
        if (ev.Particolarita.has_value()) item["Particolarita"] = *ev.Particolarita;
        else item["Particolarita"] = nullptr;
        j.push_back(item);
    }

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        error = "Impossibile aprire file in scrittura: " + filename;
        return false;
    }

    try {
        ofs << std::setw(4) << j << std::endl;
    } catch (const std::exception& e) {
        error = std::string("Errore durante la scrittura JSON: ") + e.what();
        return false;
    }
    return true;
}

std::string JsonManager::toJsonString(const std::vector<Evento>& eventi) {
    json j = json::array();
    for (const auto& ev : eventi) {
        json item;
        item["id"] = ev.id;
        item["momentoInizio"] = formatISO8601(ev.momentoInizio);
        item["priorita"] = ev.priorita;
        item["nome"] = ev.nome;
        if (ev.inizio.has_value()) item["inizio"] = *ev.inizio;
        else item["inizio"] = nullptr;
        if (ev.fine.has_value()) item["fine"] = *ev.fine;
        else item["fine"] = nullptr;
        if (ev.momentoFine.has_value()) item["momentoFine"] = formatISO8601(*ev.momentoFine);
        else item["momentoFine"] = nullptr;
        if (ev.Descrizione.has_value()) item["Descrizione"] = *ev.Descrizione;
        else item["Descrizione"] = nullptr;
        if (ev.Particolarita.has_value()) item["Particolarita"] = *ev.Particolarita;
        else item["Particolarita"] = nullptr;
        j.push_back(item);
    }
    return j.dump(4);
}
