#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "EventModel.h"
#include "NewEventiDialog.h"
#include "DayWeekView.h"
#include "../utils/util_DateTime.h"
#include "../json/jsonManager.hpp"
#include "EventDelegate.h"
#include "DayWeekView.h"

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MainWindow::MainWindow(QSqlDatabase db, int userId,
                       const QString& name, const QString& email,
                       QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_db(db),
      m_userId(userId),
      m_userName(name),
      m_userEmail(email)
{
    qDebug() << "[MainWindow] Inizio costruttore";

    ui->setupUi(this);

    // MODEL
    m_eventModel = new EventModel(this);

    // DAYWEEKVIEW (promosso da UI)
    m_dayWeekView = ui->dayWeekPlaceholder;
    m_dayWeekView->setModel(m_eventModel);
    //connect(ui->QCalendarWidget, &QCalendarWidget::clicked,
     //   this, &MainWindow::onLabelCalendarClicked);


    // LISTA EVENTI
    ui->eventsListView->setModel(m_eventModel);
    ui->eventsListView->setItemDelegate(new EventDelegate(this));
    ui->eventsListView->setSpacing(6);
    ui->eventsListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventsListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);


    // SIGNALS
    connect(ui->addEventButton, &QPushButton::clicked,
            this, &MainWindow::onNewEvent);

    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged,
            this, &MainWindow::onCalendarDateChanged);

    connect(m_dayWeekView, &DayWeekView::daySelected,
            this, &MainWindow::onDaySelectedFromView);

    connect(ui->deleteEventButton, &QPushButton::clicked,
        this, &MainWindow::onDeleteEventClicked);


    connect(ui->filterTypeCombo, &QComboBox::currentTextChanged,
        this, &MainWindow::onFilterTypeChanged);

        connect(this, &MainWindow::onDeleteEventClicked,
        this, &MainWindow::updateEventsForCurrentMode);


    // MENU FILE
    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* importJsonAction = new QAction("Importa eventi JSON", this);
    fileMenu->addAction(importJsonAction);
    connect(importJsonAction, &QAction::triggered,
            this, &MainWindow::onImportJson);

    loadEventsFromDb();
    onCalendarDateChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::sqlDate(const dateTime& dt) const
{
    QString s = QString::fromStdString(dt.getDateTime());
    return s.mid(0, 10);
}

QString MainWindow::sqlTime(const orario& o) const
{
    return QString("%1:%2:%3")
        .arg(o.getHour(), 2, 10, QChar('0'))
        .arg(o.getMin(),  2, 10, QChar('0'))
        .arg(o.getSec(),  2, 10, QChar('0'));
}

/*bool MainWindow::insertEvento(const std::shared_ptr<evento>& baseEv)
{
    auto el = std::dynamic_pointer_cast<eventoLungo>(baseEv);
    if (!el) return false;

    dateTime dtStart = el->getMomentoInizio();
    dateTime dtEnd   = el->getMomentoFine();
    orario oStart    = el->getInizioOR();
    orario oEnd      = el->getFineOR();

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO events
        (DataInizio, DataFine, Priorita, Nome, Descrizione,
         OrarioInizio, OrarioFine, Proprietario)
        VALUES (:di, :df, :prio, :nome, :desc, :oi, :of, :uid)
    )");

    q.bindValue(":di", sqlDate(dtStart));
    q.bindValue(":df", sqlDate(dtEnd));
    q.bindValue(":prio", (int)el->getPriorita());
    q.bindValue(":nome", QString::fromStdString(el->getNome()));
    q.bindValue(":desc", QString::fromStdString(el->getDescrizione()));
    q.bindValue(":oi", sqlTime(oStart));
    q.bindValue(":of", sqlTime(oEnd));
    q.bindValue(":uid", m_userId);

    if (!q.exec()) {
        QMessageBox::warning(this, "Errore", "Errore inserimento evento:\n" + q.lastError().text());
        return false;
    }

    int newId = q.lastInsertId().toInt();
    el->setId(newId);

    m_eventModel->addEvent(el);
    return true;
}*/

    void MainWindow::loadEventsFromDb()
{
    if (!m_db.isOpen()) {
        qDebug() << "[loadEventsFromDb] DB non aperto";
        return;
    }

    m_eventModel->clear();

    QSqlQuery q(m_db);
    q.prepare("SELECT id, name, start_datetime, end_datetime "
              "FROM events WHERE user_id = :uid ORDER BY start_datetime");
    q.bindValue(":uid", m_userId);

    if (!q.exec()) {
        qDebug() << "[loadEventsFromDb] ERRORE QUERY:" << q.lastError().text();
        return;
    }else {
        qDebug() << "[loadEventsFromDb] QUERY=" << q.lastQuery();
    }

    while (q.next()) {
        int id = q.value("id").toInt();
        QString name = q.value("name").toString();
        QString start = q.value("start_datetime").toString();
        QString end   = q.value("end_datetime").toString();

        dateTime dtStart = qStringToDateTime(start);
        dateTime dtEnd   = qStringToDateTime(end);

        orario oStart(0,0,0);
        orario oEnd(0,0,0);

        auto ev = std::make_shared<eventoLungo>(
            id,
            dtStart,
            1, // priorità fissa (non esiste nel DB)
            name.toStdString(),
            dtEnd,
            "", // descrizione non esiste nel DB
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
    }
    qDebug() << "[LoadEventsFromDb] caricamento eseguito = " << m_db.isOpen();
    m_dayWeekView->update();
}


bool MainWindow::insertEvento(const std::shared_ptr<evento>& baseEv)
{
    auto el = std::dynamic_pointer_cast<eventoLungo>(baseEv);
    if (!el) return false;

    dateTime dtStart = el->getMomentoInizio();
    dateTime dtEnd   = el->getMomentoFine();

    QString startStr = QString::fromStdString(dtStart.getDateTime());
    QString endStr   = QString::fromStdString(dtEnd.getDateTime());
    qDebug() << "[insertEvento] startStr = " << startStr;
    qDebug() << "[insertEvento] endStr = " << endStr;

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO events (user_id, name, start_datetime, end_datetime) "
              "VALUES (:uid, :name, :start, :end)");

    q.bindValue(":uid", m_userId);
    q.bindValue(":name", QString::fromStdString(el->getNome()));
    q.bindValue(":start", startStr);
    q.bindValue(":end", endStr);

    if (!q.exec()) {
        QMessageBox::warning(this, "Errore", "Errore inserimento evento:\n" + q.lastError().text());
        return false;
    }

    el->setId(q.lastInsertId().toInt());
    m_eventModel->addEvent(el);
    return true;
}



void MainWindow::loadEventsForDate(const QDate& date)
{
    m_dayWeekView->setDate(
        dateTime(date.year(), date.month() - 1, date.day(), 0, 0, 0)
    );
    m_dayWeekView->update();
}

void MainWindow::onNewEvent()
{
    NewEventDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto ev = dlg.createEvento();
        if (ev && insertEvento(ev)) {
            loadEventsFromDb();
            onCalendarDateChanged();
        }
    }
}


void MainWindow::onModeChanged(int index)
{
    qDebug() << "[FLOW] Cambio modalità:" << index;

    QString text = ui->modeCombo->itemText(index);

    if (text == "Day")      m_dayWeekView->setMode(DayWeekView::Mode::Day);
    else if (text == "Week")  m_dayWeekView->setMode(DayWeekView::Mode::Week);
    else if (text == "Month") m_dayWeekView->setMode(DayWeekView::Mode::Month);
    else if (text == "Year")  m_dayWeekView->setMode(DayWeekView::Mode::Year);

    m_dayWeekView->update();

    m_dayWeekView->setMode(static_cast<DayWeekView::Mode>(index));
    qDebug() << "[UPDATE] DayWeekView modalità impostata";

    updateAllLabelsForCurrentMode();
    qDebug() << "[UPDATE] Tutte le label aggiornate dopo cambio modalità";
}

void MainWindow::on_calendarWidget_currentPageChanged(int year, int month)
{
    qDebug() << "[FLOW] Cambio mese nel QCalendarWidget:" << year << month;

    QDate d(year, month, 1);
    m_dayWeekView->setDate(dateTime(year, month, 1, 0, 0, 0));
    qDebug() << "[UPDATE] DayWeekView impostato al primo giorno del mese"<< month;

    updateAllLabelsForCurrentMode();
    qDebug() << "[UPDATE] Tutte le label aggiornate per la modalità corrente";
}




void MainWindow::onCalendarDateChanged()
{
    QDate d = ui->calendarWidget->selectedDate();
    loadEventsForDate(d);
}


void MainWindow::onDaySelectedFromView(const QDate& date)
{
    qDebug() << "[FLOW] Giorno selezionato nella DayWeekView:" << date;
    // Aggiorna labelCalendar
    updateCalendarLabel(date);
        qDebug() << "[UPDATE] labelCalendar aggiornato a:" << ui->labelCalendar->text();

    // Aggiorna labelView (sincronizzata con labelCalendar)
    ui->labelView->setText(ui->labelCalendar->text());

    // --- AGGIORNA labelEvents ---
    QString formatted = date.toString("dd MMMM yyyy");
    if (!formatted.isEmpty())
        formatted[0] = formatted[0].toUpper();


    updateLabelEvents(date);
    qDebug() << "[UPDATE] labelEvents aggiornato";

    ui->labelView->setText(ui->labelCalendar->text());
    qDebug() << "[UPDATE] labelView sincronizzato con labelCalendar";

    m_dayWeekView->setDate(dateTime(date.year(), date.month(), date.day(), 0, 0, 0));
    qDebug() << "[UPDATE] DayWeekView aggiornato alla data selezionata";

    // Recupera gli eventi del giorno
    dateTime dt(date.year(), date.month(), date.day(), 0, 0, 0);
    auto events = m_eventModel->eventsForDate(dt);

    if (events.empty()) {
        ui->labelEvents->setText("Nessun evento per " + formatted);
    } else {
        ui->labelEvents->setText(
            QString("Eventi del %1 (%2)")
                .arg(formatted)
                .arg(events.size())
        );
    }
}



void MainWindow::onImportJson()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Importa JSON", QString(), "JSON (*.json)"
    );
    if (fileName.isEmpty()) return;

    std::string error;
    auto eventi = jm::JsonManager::importFromFile(fileName.toStdString(), error);

    if (!error.empty()) {
        QMessageBox::warning(this, "Errore JSON", QString::fromStdString(error));
        return;
    }

    int imported = 0;

    for (const auto& ev : eventi)
    {
        dateTime dtStart = chronoToDateTime(ev.momentoInizio);
        dateTime dtEnd   = ev.momentoFine.has_value()
                           ? chronoToDateTime(*ev.momentoFine)
                           : dtStart;

        auto nuovo = std::make_shared<eventoLungo>(
            -1,
            dtStart,
            ev.priorita,
            ev.nome,
            dtEnd,
            "",
            orario(0,0,0),
            orario(0,0,0)
        );

        if (insertEvento(nuovo))
            imported++;
    }

    QMessageBox::information(
        this,
        "Importazione completata",
        QString("%1 eventi importati").arg(imported)
    );

    loadEventsFromDb();
    onCalendarDateChanged();
}

void MainWindow::updateCalendarLabel(const QDate& date)
{
    QString formatted = date.toString("dddd dd MMMM yyyy");
    formatted[0] = formatted[0].toUpper(); // Prima lettera maiuscola

    
    QString text = "📅  " + formatted;

    ui->labelCalendar->setText(text);
    ui->labelView->setText(text);
    ui->labelView->setText(ui->labelCalendar->text());
}

void MainWindow::updateCalendarLabelMode(DayWeekView::Mode mode, const QDate& date)
{
    QString icon;
    QString text;

    switch (mode) {
    case DayWeekView::Mode::Day:
        icon = "📅";
        text = date.toString("dddd dd MMMM yyyy");
        break;

    case DayWeekView::Mode::Week:
        icon = "🗓️";
        text = QString("Settimana %1 - %2")
               .arg(date.addDays(-(date.dayOfWeek()-1)).toString("dd MMM"))
               .arg(date.addDays(7 - date.dayOfWeek()).toString("dd MMM yyyy"));
        break;

    case DayWeekView::Mode::Month:
        icon = "📆";
        text = date.toString("MMMM yyyy");
        break;

    case DayWeekView::Mode::Year:
        icon = "📘";
        text = QString::number(date.year());
        break;
    }
    QString final = icon + "  " + text;

    ui->labelCalendar->setText(final);
    ui->labelView->setText(final); 

    // colore dinamico solo in vista mese
    if (mode == DayWeekView::Mode::Month) {
        const bool hasEv = monthHasEvents(date);
        const QString style = hasEv
            ? "QLabel { color: #0A74DA; font-weight: bold; }"
            : "QLabel { color: #555; }";

        ui->labelCalendar->setStyleSheet(style);
        ui->labelView->setStyleSheet(style);
    }

    ui->labelView->setText(ui->labelCalendar->text());

}

void MainWindow::updateCalendarLabelColor(const QDate& date)
{
    bool hasEvents = false;

    if (m_eventModel) {
        dateTime dt(date.year(), date.month(), date.day(), 0, 0, 0);
        auto events = m_eventModel->eventsForDate(dt);
        hasEvents = !events.empty();
    }

    if (hasEvents) {
        ui->labelCalendar->setStyleSheet(
            "QLabel { color: #0A74DA; font-weight: bold; }"
        );
    } else {
        ui->labelCalendar->setStyleSheet(
            "QLabel { color: #555; }"
        );
    }
}

bool MainWindow::monthHasEvents(const QDate& date)
{
    if (!m_eventModel) return false;

    int year  = date.year();
    int month = date.month();
    int days  = QDate(year, month, 1).daysInMonth();

    for (int d = 1; d <= days; ++d) {
        dateTime dt(year, month, d, 0, 0, 0);
        if (!m_eventModel->eventsForDate(dt).empty())
            return true;
    }
    return false;
}
void MainWindow::updateEventsForCurrentMode()
{
    DayWeekView::Mode m = m_dayWeekView->mode();
    QDate base = m_dayWeekView->selectedDate();

    switch (m) {

    case DayWeekView::Mode::Day:
        updateLabelEvents(base);
        break;

    case DayWeekView::Mode::Week: {
        QDate monday = base.addDays(-(base.dayOfWeek() - 1));
        updateLabelEvents(monday);
        break;
    }

    case DayWeekView::Mode::Month: {
        QDate first(base.year(), base.month(), 1);
        updateLabelEvents(first);
        break;
    }

    case DayWeekView::Mode::Year: {
        QDate first(base.year(), 1, 1);
        updateLabelEvents(first);
        break;
    }
    }
}


void MainWindow::updateLabelEvents(const QDate& date)
{
    dateTime dt(date.year(), date.month(), date.day(), 0, 0, 0);
    auto events = m_eventModel->eventsForDate(dt);

    QString formatted = date.toString("dd MMMM yyyy");
    if (!formatted.isEmpty())
        formatted[0] = formatted[0].toUpper();

    if (events.empty()) {
        ui->labelEvents->setText("Nessun evento per " + formatted);
    } else {
        ui->labelEvents->setText(
            QString("Eventi del %1 (%2)")
                .arg(formatted)
                .arg(events.size())
        );
    }
}

void MainWindow::updateModeFeedback()
{
    DayWeekView::Mode m = m_dayWeekView->mode();
    QDate d = m_dayWeekView->selectedDate();

    QString modeText;
    QString icon;

    switch (m) {
    case DayWeekView::Mode::Day:
        icon = "📅";
        modeText = "Giorno aggiornato a " + d.toString("MMMM yyyy");
        break;

    case DayWeekView::Mode::Week:
        icon = "🗓️";
        modeText = "Settimana aggiornata a " + d.toString("MMMM yyyy");
        break;

    case DayWeekView::Mode::Month:
        icon = "📆";
        modeText = "Mese aggiornato a " + d.toString("MMMM yyyy");
        break;

    case DayWeekView::Mode::Year:
        icon = "📘";
        modeText = "Anno aggiornato a " + QString::number(d.year());
        break;
    }

    // Feedback visivo su labelView
    ui->labelView->setText(icon + "  " + modeText);

    // Colore dinamico se il mese ha eventi
    if (monthHasEvents(d)) {
        ui->labelView->setStyleSheet("color: #0A74DA; font-weight: bold;");
    } else {
        ui->labelView->setStyleSheet("color: #555;");
    }
}

void MainWindow::updateAllLabelsForCurrentMode()
{
    qDebug() << "[FLOW] updateAllLabelsForCurrentMode()";

    DayWeekView::Mode m = m_dayWeekView->mode();
    QDate d = m_dayWeekView->selectedDate();

    // --- Aggiorna labelCalendar e labelView ---
    updateCalendarLabelMode(m, d);
    qDebug() << "[UPDATE] labelCalendar aggiornato per modalità:" << ui->labelCalendar->text();;

    ui->labelView->setText(ui->labelCalendar->text());
    qDebug() << "[UPDATE] labelView sincronizzato";

    updateLabelEvents(d);
    qDebug() << "[UPDATE] labelEvents aggiornato";
}

void MainWindow::onLabelCalendarClicked()
{
    QDate d = m_dayWeekView->selectedDate();

    if (!d.isValid())
        d = QDate::currentDate();

    // esempio: vai al mese successivo
    QDate next = d.addMonths(1);

    ui->calendarWidget->setCurrentPage(next.year(), next.month());

    // forza l’aggiornamento completo
    m_dayWeekView->setDate(dateTime(next.year(), next.month(), 1, 0, 0, 0));
    updateAllLabelsForCurrentMode();
}


void MainWindow::onDeleteEventClicked()
{
    QString name = QInputDialog::getText(this, "Elimina evento", "Nome evento:");
    if (name.isEmpty()) return;

    QString dateStr = QInputDialog::getText(this, "Elimina evento", "Data (YYYY-MM-DD):");
    QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!date.isValid()) {
        QMessageBox::warning(this, "Errore", "Data non valida.");
        return;
    }

    QSqlQuery q(m_db);
    q.prepare(R"(
        DELETE FROM events
        WHERE user_id = :uid
          AND name = :name
          AND date(start_datetime) <= :d
          AND date(end_datetime) >= :d
    )");

    q.bindValue(":uid", m_userId);
    q.bindValue(":name", name);
    q.bindValue(":d", date.toString(Qt::ISODate));

    if (!q.exec()) {
        QMessageBox::critical(this, "Errore DB", q.lastError().text());
        return;
    }

    if (q.numRowsAffected() == 0) {
        QMessageBox::information(this, "Nessun evento", "Nessun evento trovato.");
        return;
    }

    QMessageBox::information(this, "OK", "Evento eliminato.");

    loadEventsForVisibleRange();
}


void MainWindow::onFilterTypeChanged(const QString& type)
{
    if (!m_db.isOpen()) return;
    if (!m_eventModel) return;

    if (type == "Tutti" || type.isEmpty()) {
        loadEventsForVisibleRange();
        return;
    }

    QDate base = ui->calendarWidget->selectedDate();
    if (!base.isValid())
        base = QDate::currentDate();

    QDate start(base.year(), base.month(), 1);
    QDate end = start.addMonths(1).addDays(-1);

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, name, start_datetime, end_datetime
        FROM events
        WHERE user_id = :uid
          AND name LIKE :filter
          AND date(start_datetime) <= :end
          AND date(end_datetime) >= :start
    )");

    q.bindValue(":uid", m_userId);
    q.bindValue(":filter", "%" + type + "%");
    q.bindValue(":start", start.toString(Qt::ISODate));
    q.bindValue(":end", end.toString(Qt::ISODate));

    if (!q.exec()) {
        qWarning() << "Errore filtro:" << q.lastError().text();
        return;
    }

    m_eventModel->clear();

    while (q.next()) {
        int id = q.value("id").toInt();
        QString name = q.value("name").toString();
        QString sdt = q.value("start_datetime").toString();
        QString edt = q.value("end_datetime").toString();

        dateTime dtStart = qStringToDateTime(sdt);
        dateTime dtEnd   = qStringToDateTime(edt);

        orario oStart(dtStart.getHour(), dtStart.getMin(), dtStart.getSec());
        orario oEnd(dtEnd.getHour(), dtEnd.getMin(), dtEnd.getSec());

        auto ev = std::make_shared<eventoLungo>(
            id,
            dtStart,
            1,
            name.toStdString(),
            dtEnd,
            "",
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
    }

    if (m_dayWeekView)
        m_dayWeekView->update();
    updateEventsForCurrentMode();


}

void MainWindow::loadEventsForVisibleRange()
{
    if (!ui || !ui->calendarWidget) return;
    if (!m_eventModel) return;
    if (!m_db.isOpen()) return;

    // Calcolo del mese visibile
    QDate base = ui->calendarWidget->selectedDate();
    if (!base.isValid())
        base = QDate::currentDate();

    QDate start(base.year(), base.month(), 1);
    QDate end = start.addMonths(1).addDays(-1);

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, name, start_datetime, end_datetime
        FROM events
        WHERE user_id = :uid
          AND date(start_datetime) <= :end
          AND date(end_datetime) >= :start
    )");

    q.bindValue(":uid", m_userId);
    q.bindValue(":start", start.toString(Qt::ISODate));
    q.bindValue(":end", end.toString(Qt::ISODate));

    if (!q.exec()) {
        qWarning() << "Errore query loadEventsForVisibleRange:" << q.lastError().text();
        return;
    }

    m_eventModel->clear();

    while (q.next()) {
        int id = q.value("id").toInt();
        QString name = q.value("name").toString();
        QString sdt = q.value("start_datetime").toString();
        QString edt = q.value("end_datetime").toString();

        dateTime dtStart = qStringToDateTime(sdt);
        dateTime dtEnd   = qStringToDateTime(edt);

        orario oStart(dtStart.getHour(), dtStart.getMin(), dtStart.getSec());
        orario oEnd(dtEnd.getHour(), dtEnd.getMin(), dtEnd.getSec());

        auto ev = std::make_shared<eventoLungo>(
            id,
            dtStart,
            1, // priorità default
            name.toStdString(),
            dtEnd,
            "", // descrizione vuota
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
    }

    if (m_dayWeekView)
        m_dayWeekView->update();

    updateEventsForCurrentMode();


}


