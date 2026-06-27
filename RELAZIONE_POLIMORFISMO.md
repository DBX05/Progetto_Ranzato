# Relazione tecnica - Uso non banale del polimorfismo nella gerarchia impegno

## 1. Obiettivo del vincolo
Il progetto richiede un uso del polimorfismo non banale.
Un uso banale sarebbe limitarsi a distruttori virtuali o getter.
In questo lavoro il polimorfismo e usato per ottenere comportamenti differenti e osservabili in interfaccia, in base al tipo dinamico dell'oggetto.

## 2. Contesto del dominio
La gerarchia principale contiene i seguenti tipi:
- impegno (classe base astratta)
- evento
- eventoLungo
- raggruppa
- festivita
- compleanno
- riunione
- altroTipo

Nel modello Qt degli eventi, gli elementi sono gestiti in una collezione di puntatori a base comune (eventoLungo).
Questo e il punto in cui il dispatch dinamico produce comportamento diverso senza if/else basati sul tipo.

## 3. Scelta progettuale
Sono state introdotte nella classe base due operazioni astratte orientate alla UI:
- buildUiSummary(): costruisce il testo sintetico da visualizzare
- suggestedAction(): restituisce l'azione suggerita all'utente

Queste funzioni sono state dichiarate pure virtual nella base e ridefinite in tutte le sottoclassi concrete.

## 4. Implementazione
### 4.1 Contratto polimorfo
Nella base impegno:
- virtual std::string buildUiSummary() const = 0;
- virtual std::string suggestedAction() const = 0;

Questa scelta forza ogni tipo concreto a definire la propria logica.

### 4.2 Override specifici
Ogni sottoclasse fornisce una semantica diversa:

- evento:
  - riepilogo con nome + intervallo orario breve
  - azione: apertura dettaglio evento

- eventoLungo:
  - riepilogo con intervallo temporale esteso + descrizione
  - azione: apertura timeline estesa

- raggruppa:
  - riepilogo da contenitore (numero eventi e primo elemento)
  - azione: espansione gruppo e filtro sottovoci

- festivita:
  - riepilogo per evento giornaliero ricorrente
  - azione: impostazione promemoria annuale

- compleanno:
  - riepilogo con numero invitati
  - azione: invio auguri o inviti

- riunione:
  - riepilogo con numero partecipanti e fascia temporale
  - azione: invio ordine del giorno

- altroTipo:
  - riepilogo personalizzato tramite campo Particolarita
  - azione: apertura workflow personalizzato

### 4.3 Integrazione nel modello Qt
Nel metodo data() del modello:
- Qt::DisplayRole usa buildUiSummary()
- DescriptionRole usa suggestedAction()

Quindi la vista ottiene contenuti differenti con la stessa chiamata su puntatore base.

## 5. Flusso di esecuzione (dispatch dinamico)
1. Il modello contiene un elemento come puntatore a tipo base.
2. La UI chiede il dato da visualizzare.
3. Il modello invoca buildUiSummary() o suggestedAction() sull'interfaccia comune.
4. A runtime viene selezionato l'override corretto in base al tipo reale (es. riunione, compleanno, festivita).
5. La UI mostra un risultato realmente diverso senza logica condizionale centralizzata sui tipi.

## 6. Perche l'uso e non banale
- Le funzioni polimorfe non espongono solo stato: orchestrano comportamento applicativo.
- L'effetto e visibile in UI (testo mostrato e azione proposta).
- Il codice client resta stabile: aggiungere un nuovo tipo richiede solo nuovi override, senza modifiche invasive al modello.

## 7. Benefici ottenuti
- Estendibilita: aggiunta di nuovi tipi con impatto ridotto.
- Coesione: ogni classe gestisce la propria semantica.
- Manutenibilita: meno if/switch su tipi concreti.
- Chiarezza architetturale: contratto esplicito per presentazione e azione.

## 8. Limiti e miglioramenti futuri
- Le stringhe UI sono attualmente hardcoded nelle classi dominio.
  Miglioria: introdurre una strategia di formattazione separata per localizzazione.
- Le azioni suggerite sono solo testuali.
  Miglioria: mappare suggestedAction() a comandi applicativi concreti.
- Possibile evoluzione con pattern Strategy o Visitor per gestire rendering multi-canale (lista, calendario, esportazione).

## 9. Conclusione
Il vincolo di polimorfismo non banale e soddisfatto: la stessa interfaccia astratta produce comportamenti distinti e significativi in funzione del tipo dinamico, con impatto diretto sulla visualizzazione e sulle azioni offerte all'utente.
