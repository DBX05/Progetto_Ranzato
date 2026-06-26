// MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "EventModel.h"
#include "NewEventiDialog.h" // Assumed corrected include name
#include "DayWeekView.h"
#include "../utils/util_DateTime.h"
#include "../json/jsonManager.hpp" // Assumed correct path
#include "EventDelegate.h"

#include <QCalendarWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

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
    qDebug() << "[MainWindow] Initializing constructor";

    ui->setupUi(this);

    // --- Model and Views Initialization ---
    m_eventModel = new EventModel(this);

    // DayWeekView (promoted from UI)
    m_dayWeekView = ui->dayWeekPlaceholder;
    m_dayWeekView->setModel(m_eventModel);

    // Events List View
    ui->eventsListView->setModel(m_eventModel);
    ui->eventsListView->setItemDelegate(new EventDelegate(this));
    ui->eventsListView->setSpacing(6);
    ui->eventsListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventsListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // --- Signals and Slots Connections ---

    // Event management
    connect(ui->addEventButton, &QPushButton::clicked, this, &MainWindow::onNewEvent);
    connect(ui->deleteEventButton, &QPushButton::clicked, this, &MainWindow::onDeleteEventClicked);

    // Calendar and View navigation
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &MainWindow::onCalendarDateChanged);
    connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged, this, &MainWindow::on_calendarWidget_currentPageChanged);
    connect(ui->filterTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onFilterTypeChanged);
    m_dayWeekView->setCalendarWidget(ui->calendarWidget);

    // --- Menu Bar Setup ---
    setupFileMenu();
    setupStackedPages();

    // --- Initial Data Load and UI Update ---
    loadEventsFromDb();
    updateViewForCurrentMonth(); // Initial view setup for current month
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupFileMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* importJsonAction = new QAction("Importa eventi JSON", this);
    fileMenu->addAction(importJsonAction);
    connect(importJsonAction, &QAction::triggered, this, &MainWindow::onImportJson);
}

void MainWindow::setupStackedPages()
{
    m_stack = new QStackedWidget(this);
    m_mainPage = ui->centralwidget;
    m_stack->addWidget(m_mainPage);
    setCentralWidget(m_stack);

    m_createPage = new QWidget(this);
    auto* createLayout = new QVBoxLayout(m_createPage);
    createLayout->setContentsMargins(24, 24, 24, 24);
    createLayout->setSpacing(12);

    auto* createTitle = new QLabel("Nuovo evento", m_createPage);
    createTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    createLayout->addWidget(createTitle);

    auto* createForm = new QFormLayout();
    createForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    createForm->setSpacing(8);

    m_newEventNameEdit = new QLineEdit(m_createPage);
    m_newEventNameEdit->setPlaceholderText("Titolo dell'evento");
    createForm->addRow("Nome:", m_newEventNameEdit);

    m_newEventDateEdit = new QDateEdit(QDate::currentDate(), m_createPage);
    m_newEventDateEdit->setCalendarPopup(true);
    m_newEventDateEdit->setDisplayFormat("yyyy-MM-dd");
    createForm->addRow("Data:", m_newEventDateEdit);

    m_newEventStartEdit = new QTimeEdit(QTime::currentTime(), m_createPage);
    m_newEventStartEdit->setDisplayFormat("HH:mm");
    createForm->addRow("Ora inizio:", m_newEventStartEdit);

    m_newEventEndEdit = new QTimeEdit(QTime::currentTime().addSecs(3600), m_createPage);
    m_newEventEndEdit->setDisplayFormat("HH:mm");
    createForm->addRow("Ora fine:", m_newEventEndEdit);

    m_newEventTypeCombo = new QComboBox(m_createPage);
    m_newEventTypeCombo->addItem("Generico", 0);
    m_newEventTypeCombo->addItem("Compleanno", 1);
    m_newEventTypeCombo->addItem("Riunione", 2);
    m_newEventTypeCombo->addItem("Raggruppa", 3);
    createForm->addRow("Tipo:", m_newEventTypeCombo);

    createLayout->addLayout(createForm);

    auto* createButtons = new QHBoxLayout();
    auto* saveButton = new QPushButton("Salva", m_createPage);
    auto* cancelCreateButton = new QPushButton("Annulla", m_createPage);
    createButtons->addStretch();
    createButtons->addWidget(saveButton);
    createButtons->addWidget(cancelCreateButton);
    createLayout->addLayout(createButtons);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onCreateEventSubmit);
    connect(cancelCreateButton, &QPushButton::clicked, this, &MainWindow::showMainPage);

    m_deletePage = new QWidget(this);
    auto* deleteLayout = new QVBoxLayout(m_deletePage);
    deleteLayout->setContentsMargins(24, 24, 24, 24);
    deleteLayout->setSpacing(12);

    auto* deleteTitle = new QLabel("Elimina evento", m_deletePage);
    deleteTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    deleteLayout->addWidget(deleteTitle);

    auto* deleteForm = new QFormLayout();
    deleteForm->setSpacing(8);

    m_deleteEventNameEdit = new QLineEdit(m_deletePage);
    m_deleteEventNameEdit->setPlaceholderText("Nome evento");
    deleteForm->addRow("Nome:", m_deleteEventNameEdit);

    m_deleteEventDateEdit = new QDateEdit(QDate::currentDate(), m_deletePage);
    m_deleteEventDateEdit->setCalendarPopup(true);
    m_deleteEventDateEdit->setDisplayFormat("yyyy-MM-dd");
    deleteForm->addRow("Data:", m_deleteEventDateEdit);

    deleteLayout->addLayout(deleteForm);

    auto* deleteButtons = new QHBoxLayout();
    auto* deleteButton = new QPushButton("Elimina", m_deletePage);
    auto* cancelDeleteButton = new QPushButton("Annulla", m_deletePage);
    deleteButtons->addStretch();
    deleteButtons->addWidget(deleteButton);
    deleteButtons->addWidget(cancelDeleteButton);
    deleteLayout->addLayout(deleteButtons);

    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteEventSubmit);
    connect(cancelDeleteButton, &QPushButton::clicked, this, &MainWindow::showMainPage);

    m_stack->addWidget(m_createPage);
    m_stack->addWidget(m_deletePage);
    m_stack->setCurrentWidget(m_mainPage);
}

void MainWindow::showMainPage()
{
    if (m_stack) {
        m_stack->setCurrentWidget(m_mainPage);
    }
}

void MainWindow::showCreateEventPage()
{
    if (m_stack) {
        m_stack->setCurrentWidget(m_createPage);
    }
}

void MainWindow::showDeleteEventPage()
{
    if (m_stack) {
        m_stack->setCurrentWidget(m_deletePage);
    }
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

void MainWindow::loadEventsFromDb()
{
    qDebug() << "[loadEventsFromDb] Start";
    qDebug() << "[loadEventsFromDb] userId=" << m_userId << "dbOpen=" << m_db.isOpen();

    if (!m_db.isOpen()) {
        qDebug() << "[loadEventsFromDb] Database not open.";
        return;
    }

    m_eventModel->clear(); // Clear existing events before loading
    qDebug() << "[loadEventsFromDb] Event model cleared.";

    QSqlQuery q(m_db);
    q.prepare("SELECT id, name, start_datetime, end_datetime "
              "FROM events WHERE user_id = :uid ORDER BY start_datetime");
    q.bindValue(":uid", m_userId);
    qDebug() << "[loadEventsFromDb] Prepared query for user" << m_userId;

    if (!q.exec()) {
        qDebug() << "[loadEventsFromDb] Query execution failed:" << q.lastError().text();
        return;
    } else {
        qDebug() << "[loadEventsFromDb] Query executed successfully. Query:" << q.lastQuery();
    }

    int rowIndex = 0;
    while (q.next()) {
        ++rowIndex;
        int id = q.value("id").toInt();
        QString name = q.value("name").toString();
        QString start = q.value("start_datetime").toString();
        QString end   = q.value("end_datetime").toString();

        qDebug() << "[loadEventsFromDb] Row" << rowIndex << "id=" << id
                 << "name=" << name
                 << "start=" << start
                 << "end=" << end;

        dateTime dtStart = qStringToDateTime(start);
        dateTime dtEnd   = qStringToDateTime(end);
        qDebug() << "[loadEventsFromDb] Parsed dtStart=" << QString::fromStdString(dtStart.getDateTime())
                 << "dtEnd=" << QString::fromStdString(dtEnd.getDateTime());

        if (dtStart.getDateTime().empty() || dtEnd.getDateTime().empty()) {
            qDebug() << "[loadEventsFromDb] Invalid parsed date detected for row" << rowIndex;
            qDebug() << "[loadEventsFromDb] Falling back to default values for this row.";
            continue;
        }

        // Default orario if not available from DB (adjust as needed)
        orario oStart(dtStart.getHour(), dtStart.getMin(), dtStart.getSec());
        orario oEnd(dtEnd.getHour(), dtEnd.getMin(), dtEnd.getSec());

        auto ev = std::make_shared<eventoLungo>(
            id,
            dtStart,
            1, // Default priority if not in DB
            name.toStdString(),
            dtEnd,
            "", // Default description if not in DB
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
        qDebug() << "[loadEventsFromDb] Added event to model. Total now=" << m_eventModel->rowCount();
    }

    qDebug() << "[LoadEventsFromDb] Loaded" << m_eventModel->rowCount() << "events.";
    qDebug() << "[LoadEventsFromDb] Finished successfully.";
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
    qDebug() << "[insertEvento] Inserting event. Start:" << startStr << "End:" << endStr;

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
    m_eventModel->addEvent(el); // Add to model after successful DB insertion
    qDebug() << "[insertEvento] Event inserted successfully. New ID:" << el->getId();
    return true;
}

void MainWindow::loadEventsForDate(const QDate& date)
{
    m_dayWeekView->updateLabelEventsForDate(date);
    m_dayWeekView->update();
}

void MainWindow::onNewEvent()
{
    showCreateEventPage();
}

void MainWindow::onCreateEventSubmit()
{
    if (!m_newEventNameEdit || !m_newEventDateEdit || !m_newEventStartEdit || !m_newEventEndEdit || !m_newEventTypeCombo) {
        return;
    }

    QString name = m_newEventNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Errore", "Inserisci il nome dell'evento.");
        return;
    }

    QDate date = m_newEventDateEdit->date();
    QTime startTime = m_newEventStartEdit->time();
    QTime endTime = m_newEventEndEdit->time();

    QString startStr = QString("%1-%2-%3 %4:%5:00")
        .arg(date.year(), 4, 10, QChar('0'))
        .arg(date.month(), 2, 10, QChar('0'))
        .arg(date.day(), 2, 10, QChar('0'))
        .arg(startTime.hour(), 2, 10, QChar('0'))
        .arg(startTime.minute(), 2, 10, QChar('0'));

    QString endStr = QString("%1-%2-%3 %4:%5:00")
        .arg(date.year(), 4, 10, QChar('0'))
        .arg(date.month(), 2, 10, QChar('0'))
        .arg(date.day(), 2, 10, QChar('0'))
        .arg(endTime.hour(), 2, 10, QChar('0'))
        .arg(endTime.minute(), 2, 10, QChar('0'));

    dateTime dtStart = qStringToDateTime(startStr);
    dateTime dtEnd = qStringToDateTime(endStr);
    orario oStart(dtStart.getSec(), dtStart.getMin(), dtStart.getHour());
    orario oEnd(dtEnd.getSec(), dtEnd.getMin(), dtEnd.getHour());

    auto ev = std::make_shared<eventoLungo>(
        -1,
        dtStart,
        1,
        name.toStdString(),
        dtEnd,
        "",
        oStart,
        oEnd
    );

    if (insertEvento(ev)) {
        loadEventsFromDb();
        updateViewForCurrentMonth();
        showMainPage();
    }
}

void MainWindow::on_calendarWidget_currentPageChanged(int year, int month)
{
    qDebug() << "[FLOW] Calendar page changed to:" << year << month;
    updateViewForCurrentMonth();
}

void MainWindow::onCalendarDateChanged()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    qDebug() << "[FLOW] Calendar date selection changed to:" << selectedDate;

    // Update the DayWeekView to show events for the selected date
    loadEventsForDate(selectedDate);

    // Update labels to reflect the selected date
    updateCalendarLabel(selectedDate);
    updateLabelEvents(selectedDate); // Update events list label
}

void MainWindow::onDaySelectedFromView(const QDate& date)
{
    qDebug() << "[FLOW] Day selected in DayWeekView:" << date;
    updateCalendarLabel(date);
    ui->labelView->setText(ui->labelCalendar->text()); // Synchronize labelView

    updateLabelEvents(date);

    m_dayWeekView->updateLabelEventsForDate(date);
    m_dayWeekView->update(); // Ensure the view redraws to highlight the selection

    // Ensure the calendar widget reflects the selection from DayWeekView
    ui->calendarWidget->setSelectedDate(date);
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
        // Handle optional end time
        dateTime dtEnd = ev.momentoFine.has_value()
                           ? chronoToDateTime(*ev.momentoFine)
                           : dtStart; // Default to start time if not specified

        auto nuovo = std::make_shared<eventoLungo>(
            -1, // ID will be generated by DB
            dtStart,
            ev.priorita,
            ev.nome,
            dtEnd,
            "", // Description not available from this JSON structure
            orario(dtStart.getHour(), dtStart.getMin(), dtStart.getSec()), // Use start time for default orario
            orario(dtEnd.getHour(), dtEnd.getMin(), dtEnd.getSec())      // Use end time for default orario
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
    updateViewForCurrentMonth(); // Refresh the view for the current month
}

void MainWindow::updateCalendarLabel(const QDate& date)
{
    if (!date.isValid()) return;

    QString text = date.toString("dddd dd MMMM yyyy");
    if (!text.isEmpty())
        text[0] = text[0].toUpper();

    ui->labelCalendar->setText("📅  " + text);
    ui->labelView->setText(ui->labelCalendar->text()); // Keep labelView in sync
}

bool MainWindow::monthHasEvents(const QDate& date)
{
    if (!m_eventModel) return false;

    int year  = date.year();
    int month = date.month();
    // Ensure we use a valid date object for daysInMonth
    QDate firstDayOfMonth(year, month, 1);
    if (!firstDayOfMonth.isValid()) return false;
    int daysInMonth = firstDayOfMonth.daysInMonth();

    for (int d = 1; d <= daysInMonth; ++d) {
        dateTime dt(year, month - 1, d, 0, 0, 0, 0);
        if (!m_eventModel->eventsForDate(dt).empty())
            return true;
    }
    return false;
}

void MainWindow::updateLabelEvents(const QDate& date)
{
    if (!date.isValid()) {
        ui->labelEvents->setText("Eventi");
        return;
    }

    QString formatted = date.toString("dd MMMM yyyy");
    if (!formatted.isEmpty())
        formatted[0] = formatted[0].toUpper();

    ui->labelEvents->setText("Eventi del " + formatted);
}

void MainWindow::onLabelCalendarClicked()
{
    // This slot seems to be intended for an interaction with labelCalendar itself.
    // Currently, it navigates the calendar widget to the next month.
    QDate currentCalDate = ui->calendarWidget->selectedDate();
    if (!currentCalDate.isValid())
        currentCalDate = QDate::currentDate();

    QDate nextMonth = currentCalDate.addMonths(1);
    ui->calendarWidget->setCurrentPage(nextMonth.year(), nextMonth.month());

    // Update the DayWeekView to show the first day of the new month
    updateViewForCurrentMonth(); // Ensure the view reflects the new month
}

void MainWindow::onDeleteEventClicked()
{
    showDeleteEventPage();
}

void MainWindow::onDeleteEventSubmit()
{
    if (!m_deleteEventNameEdit || !m_deleteEventDateEdit) {
        return;
    }

    QString name = m_deleteEventNameEdit->text().trimmed();
    QDate date = m_deleteEventDateEdit->date();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Errore", "Inserisci il nome dell'evento da eliminare.");
        return;
    }

    if (!date.isValid()) {
        QMessageBox::warning(this, "Errore", "Seleziona una data valida.");
        return;
    }

    if (QMessageBox::question(this, "Conferma eliminazione",
                              QString("Sei sicuro di voler eliminare l'evento '%1' alla data %2?")
                                  .arg(name)
                                  .arg(date.toString(Qt::ISODate)),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
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
        QMessageBox::critical(this, "Errore DB", "Errore durante l'eliminazione dell'evento: " + q.lastError().text());
        return;
    }

    if (q.numRowsAffected() == 0) {
        QMessageBox::information(this, "Nessun evento trovato", "Nessun evento corrispondente trovato per l'eliminazione.");
    } else {
        QMessageBox::information(this, "Successo", QString("%1 evento(i) eliminato(i).").arg(q.numRowsAffected()));
        loadEventsFromDb();
        updateViewForCurrentMonth();
    }

    showMainPage();
}

void MainWindow::onFilterTypeChanged(const QString& type)
{
    qDebug() << "[FLOW] Filter type changed to:" << type;
    // Re-apply filtering or load all events.
    // Current implementation reloads events for the visible month based on the filter.
    loadEventsForVisibleRange();
    updateViewForCurrentMonth(); // Ensure the view updates with filtered data
}

// --- New Functionality: Displaying events for the current month ---

void MainWindow::updateViewForCurrentMonth()
{
    qDebug() << "[FLOW] Updating view for the current month.";

    if (!ui || !ui->calendarWidget) {
        qDebug() << "UI elements not ready.";
        return;
    }

    QDate currentDate = ui->calendarWidget->selectedDate();
    if (!currentDate.isValid()) {
        currentDate = QDate::currentDate();
        ui->calendarWidget->setSelectedDate(currentDate); // Ensure calendar is valid
    }

    // Load events relevant to the *entire* current month into the model
    loadEventsForVisibleRange();

    // Update the UI to reflect the month view
    updateCalendarLabel(currentDate);
    updateLabelEvents(currentDate); // Show events for the selected date within the month view
    m_dayWeekView->update(); // Redraw the DayWeekView

    qDebug() << "[UPDATE] DayWeekView refreshed for" << currentDate.toString("MMMM yyyy");
}

void MainWindow::loadEventsForVisibleRange()
{
    if (!m_db.isOpen()) {
        qDebug() << "[loadEventsForVisibleRange] Database not open.";
        return;
    }
    if (!m_eventModel) {
        qDebug() << "[loadEventsForVisibleRange] Event model not initialized.";
        return;
    }

    // Determine the date range for the current month view
    QDate baseDate = ui->calendarWidget->selectedDate();
    if (!baseDate.isValid()) {
        baseDate = QDate::currentDate();
        ui->calendarWidget->setSelectedDate(baseDate); // Ensure a valid date
    }

    QDate startDate(baseDate.year(), baseDate.month(), 1);
    QDate endDate = startDate.addMonths(1).addDays(-1); // Last day of the month

    QString filterType = ui->filterTypeCombo->currentText();
    bool applyFilter = (filterType != "Tutti" && !filterType.isEmpty());

    QString queryStr = R"(
        SELECT id, name, start_datetime, end_datetime
        FROM events
        WHERE user_id = :uid
          AND date(start_datetime) <= :end
          AND date(end_datetime) >= :start
        ORDER BY start_datetime
    )";

    QSqlQuery q(m_db);
    q.prepare(queryStr);
    q.bindValue(":uid", m_userId);
    q.bindValue(":start", startDate.toString(Qt::ISODate));
    q.bindValue(":end", endDate.toString(Qt::ISODate));

    if (!q.exec()) {
        qWarning() << "Error loading events for visible range:" << q.lastError().text();
        return;
    }

    m_eventModel->clear(); // Clear before loading new events

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

        if (applyFilter) {
            const std::string typeName = ev->getevName();
            bool matches = false;
            if (filterType == "Compleanno") {
                matches = (typeName == "compleann");
            } else if (filterType == "Riunione") {
                matches = (typeName == "riunione");
            } else if (filterType == "Raggruppa") {
                matches = (typeName == "raggruppa");
            } else if (filterType == "Generico") {
                matches = (typeName == "evento");
            }

            if (!matches) {
                continue;
            }
        }

        m_eventModel->addEvent(ev);
    }

    qDebug() << "[loadEventsForVisibleRange] Loaded" << m_eventModel->rowCount() << "events for the current month.";
    m_dayWeekView->update(); // Trigger redraw of the DayWeekView
    updateCalendarLabel(baseDate);
    updateLabelEvents(baseDate);
}
