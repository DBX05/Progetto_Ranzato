#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "EventModel.h"
#include "NewEventiDialog.h"
#include "DayWeekView.h"
#include "../utils/util_datetime.h"
#include "../json/jsonManager.hpp"


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
    ui->setupUi(this);

    // MODEL
    m_eventModel = new EventModel(this);

    // DAYWEEKVIEW (promosso da UI)
    m_dayWeekView = ui->dayWeekPlaceholder;
    m_dayWeekView->setModel(m_eventModel);

    // LISTA EVENTI
    ui->eventsListView->setModel(m_eventModel);

    // SIGNALS
    connect(ui->addEventButton, &QPushButton::clicked,
            this, &MainWindow::onNewEvent);

    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged,
            this, &MainWindow::onCalendarDateChanged);

    connect(m_dayWeekView, &DayWeekView::daySelected,
            this, &MainWindow::onDaySelectedFromView);

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

bool MainWindow::insertEvento(const std::shared_ptr<evento>& baseEv)
{
    auto el = std::dynamic_pointer_cast<eventoLungo>(baseEv);
    if (!el) return false;

    dateTime dtStart = el->getMomentoInizio();
    dateTime dtEnd   = el->getMomentoFine();
    orario oStart    = el->getInizioOR();
    orario oEnd      = el->getFineOR();

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO impegni
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
}

void MainWindow::loadEventsFromDb()
{
    if (!m_db.isValid() || !m_db.isOpen())
        return;

    m_eventModel->clear();

    QSqlQuery q(m_db);
    q.prepare("SELECT Id, DataInizio, DataFine, Priorita, Nome, Descrizione, OrarioInizio, OrarioFine "
              "FROM impegni WHERE Proprietario = 1 ORDER BY DataInizio");
    //q.bindValue(":uid", m_userId);

    /*if (!q.exec()) {
        qDebug() << "DB load error:" << q.lastError().text();
        return;
    }*/

    while (q.next()) {
       //int id = q.value("Id").toInt();
       int id = 1;
        QString di = q.value("DataInizio").toString();
        QString df = q.value("DataFine").toString();
        QString oi = q.value("OrarioInizio").toString();
        QString of = q.value("OrarioFine").toString();
        QString desc = q.value("Descrizione").toString();
        QString name = q.value("Nome").toString();
        int priority = q.value("Priorita").toInt();

        dateTime dtStart = qStringToDateTime(di + " " + oi);
        dateTime dtEnd   = qStringToDateTime(df + " " + of);

        orario oStart(oi.mid(6,2).toInt(), oi.mid(3,2).toInt(), oi.mid(0,2).toInt());
        orario oEnd  (of.mid(6,2).toInt(), of.mid(3,2).toInt(), of.mid(0,2).toInt());

        auto ev = std::make_shared<eventoLungo>(
            id,
            dtStart,
            static_cast<unsigned int>(priority),
            name.toStdString(),
            dtEnd,
            desc.toStdString(),
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
    }

    m_dayWeekView->update();
}

void MainWindow::loadEventsForDate(const QDate& date)
{
    Q_UNUSED(date);
    m_dayWeekView->setDate(dateTime(date.year(), date.month(), date.day(), 0, 0, 0));
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
    QString text = ui->modeCombo->itemText(index);
    if (text == "Day") {
        m_dayWeekView->setMode(DayWeekView::Mode::Day);
    } else if (text == "Week") {
        m_dayWeekView->setMode(DayWeekView::Mode::Week);
    } else if (text == "Month") {
        m_dayWeekView->setMode(DayWeekView::Mode::Month);
    } else if (text == "Year") {
        m_dayWeekView->setMode(DayWeekView::Mode::Year);
    }
    m_dayWeekView->update();
}

void MainWindow::onCalendarDateChanged()
{
    QDate d = ui->calendarWidget->selectedDate();
    m_dayWeekView->setDate(dateTime(d.year(), d.month(), d.day(), 0, 0, 0));
    m_dayWeekView->update();
}

void MainWindow::onDaySelectedFromView(const QDate& date)
{
    ui->calendarWidget->setSelectedDate(date);
    loadEventsForDate(date);
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

        dateTime dtEnd = dtStart;
        if (ev.momentoFine.has_value())
            dtEnd = chronoToDateTime(*ev.momentoFine);

        orario oStart = orarioFromOptional(ev.inizio);
        orario oEnd   = orarioFromOptional(ev.fine);

        std::string desc = ev.Descrizione.has_value() ? *ev.Descrizione : "";

        auto nuovo = std::make_shared<eventoLungo>(
            -1,
            dtStart,
            ev.priorita,
            ev.nome,
            dtEnd,
            desc,
            oStart,
            oEnd
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
