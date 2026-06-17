#include "MainWindow.h"
#include "EventModel.h"
#include "NewEventiDialog.h"
#include "DayWeekView.h"
#include "util_dateTime.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QLabel>
#include <QStringListModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QSqlDatabase db, int userId, const QString& userName, const QString& userEmail, QWidget* parent)
    : QMainWindow(parent),
      m_calendar(new QCalendarWidget(this)),
      m_listView(new QListView(this)),
      m_modeCombo(new QComboBox(this)),
      m_dayWeekView(new DayWeekView(this)),
      m_model(new EventModel(this)),
      m_db(db),
      m_userId(userId),
      m_userName(userName),
      m_userEmail(userEmail)
{
    setWindowTitle("Agenda Qt - Day/Week View Example");

    m_listView->setModel(m_model);

    m_modeCombo->addItem("Day");
    m_modeCombo->addItem("Week");
    m_modeCombo->setCurrentIndex(0);

    auto* addBtn = new QPushButton("Nuovo evento", this);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddEvent);
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this](){
        onCalendarSelectionChanged(m_calendar->selectedDate());
    });
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);

    m_dayWeekView->setModel(m_model);
    m_dayWeekView->setDate(m_calendar->selectedDate());

    auto* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(new QLabel("Calendario"));
    leftLayout->addWidget(m_calendar);
    leftLayout->addWidget(addBtn);
    leftLayout->addWidget(m_modeCombo);

    auto* leftWidget = new QWidget;
    leftWidget->setLayout(leftLayout);

    auto* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel("Vista Giorno/Settimana"));
    rightLayout->addWidget(m_dayWeekView);
    rightLayout->addWidget(new QLabel("Eventi del giorno"));
    rightLayout->addWidget(m_listView);

    auto* rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);

    auto* splitter = new QSplitter;
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    // Carica eventi dal DB per l'utente autenticato
    loadEventsFromDb();
    refreshListForDate(m_calendar->selectedDate());
}

void MainWindow::onAddEvent() {
    NewEventDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto ev = dlg.createEvento();
        if (auto longEv = std::dynamic_pointer_cast<eventoLungo>(ev)) {
            // imposta user_id se la classe lo supporta; qui salviamo nel DB con userId
            m_model->addEvent(longEv);
            saveEventToDb(longEv);
            m_dayWeekView->update();
            refreshListForDate(m_calendar->selectedDate());
        }
    }
}

void MainWindow::onCalendarSelectionChanged(const QDate& date) {
    m_dayWeekView->setDate(date);
    refreshListForDate(date);
}

void MainWindow::onModeChanged(int index) {
    m_dayWeekView->setMode(index == 0 ? DayWeekView::Day : DayWeekView::Week);
}

void MainWindow::populateSampleData() {
    // non usato: i dati vengono caricati dal DB
}

void MainWindow::refreshListForDate(const QDate& date) {
    auto events = m_model->eventsForDate(date);
    QStringList list;
    for (const auto& ev : events) {
        QString line = QString::fromStdString(ev->getNome()) + "  " +
                       QString::fromStdString(ev->getMomentoInizio().getDateTime()).mid(11,5) + " - " +
                       QString::fromStdString(ev->getMomentoFine().getDateTime()).mid(11,5);
        list << line;
    }
    auto* model = new QStringListModel(list, this);
    m_listView->setModel(model);
}

// ---------------- DB integration ----------------
void MainWindow::loadEventsFromDb() {
    if (!m_db.isValid() || !m_db.isOpen()) return;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, name, start_datetime, end_datetime, type, description, priority FROM events WHERE user_id = ? ORDER BY start_datetime");
    q.addBindValue(m_userId);
    if (!q.exec()) {
        qDebug() << "DB load error:" << q.lastError().text();
        return;
    }
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString start = q.value(2).toString();
        QString end = q.value(3).toString();
        int type = q.value(4).toInt();
        QString desc = q.value(5).toString();
        int priority = q.value(6).toInt();

        dateTime dtStart = qStringToDateTime(start);
        dateTime dtEnd = qStringToDateTime(end);

        orario oStart(dtStart.getSec(), dtStart.getMin(), dtStart.getHour());
        orario oEnd(dtEnd.getSec(), dtEnd.getMin(), dtEnd.getHour());

        auto ev = std::make_shared<eventoLungo>(id, dtStart, static_cast<unsigned int>(priority), name.toStdString(), dtEnd, desc.toStdString(), oStart, oEnd);
        m_model->addEvent(ev);
    }
    m_dayWeekView->update();
}

void MainWindow::saveEventToDb(const std::shared_ptr<eventoLungo>& ev) {
    if (!m_db.isValid() || !m_db.isOpen()) return;
    QSqlQuery q(m_db);
    QString start = dateTimeToQString(ev->getMomentoInizio());
    QString end = dateTimeToQString(ev->getMomentoFine());
    QString name = QString::fromStdString(ev->getNome());
    QString desc = QString::fromStdString(ev->getDescrizione());
    int type = ev->getType();
    int priority = static_cast<int>(ev->getPriorita());

    // Inserimento con user_id
    q.prepare("INSERT INTO events (user_id, name, start_datetime, end_datetime, type, description, priority) VALUES (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(m_userId);
    q.addBindValue(name);
    q.addBindValue(start);
    q.addBindValue(end);
    q.addBindValue(type);
    q.addBindValue(desc);
    q.addBindValue(priority);
    if (!q.exec()) {
        qDebug() << "DB insert error:" << q.lastError().text();
    } else {
        // opzionale: recupera id e aggiorna oggetto se necessario
        QVariant lastId = q.lastInsertId();
        if (lastId.isValid()) {
            // se la classe eventoLungo avesse setId, chiamala qui
        }
    }
}
