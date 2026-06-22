#include "MainWindow.h"
#include "EventModel.h"
#include "NewEventiDialog.h"
#include "DayWeekView.h"
#include "util_datetime.h"
#include "../json/jsonManager.hpp"

#include <QMenuBar>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QSqlDatabase db, int userId,
                       const QString& name, const QString& email,
                       QWidget* parent)
    : QMainWindow(parent),
      m_db(db),
      m_userId(userId),
      m_userName(name),
      m_userEmail(email)
{
    m_eventModel = new EventModel(this);
    m_dayWeekView = new DayWeekView(this);
    m_dayWeekView->setModel(m_eventModel);

    setCentralWidget(m_dayWeekView);

    QMenu* fileMenu = menuBar()->addMenu("File");
    m_importJsonAction = new QAction("Importa eventi JSON", this);
    fileMenu->addAction(m_importJsonAction);

    connect(m_importJsonAction, &QAction::triggered,
            this, &MainWindow::onImportJson);

    loadEventsForDate(QDate::currentDate());
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

void MainWindow::loadEventsForDate(const QDate& date)
{
    m_eventModel->clear();

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT Id, DataInizio, DataFine, Priorita, Nome, Descrizione,
               OrarioInizio, OrarioFine
        FROM impegni
        WHERE Proprietario = :uid
          AND DataInizio <= :d
          AND DataFine >= :d
    )");
    q.bindValue(":uid", m_userId);
    q.bindValue(":d", date.toString("yyyy-MM-dd"));

    if (!q.exec()) {
        QMessageBox::warning(this, "Errore", "Errore caricamento eventi:\n" + q.lastError().text());
        return;
    }

    while (q.next()) {
        QString di = q.value("DataInizio").toString();
        QString df = q.value("DataFine").toString();
        QString oi = q.value("OrarioInizio").toString();
        QString of = q.value("OrarioFine").toString();

        dateTime dtStart = qStringToDateTime(di + " " + oi);
        dateTime dtEnd   = qStringToDateTime(df + " " + of);

        orario oStart(
            oi.mid(0,2).toInt(),
            oi.mid(3,2).toInt(),
            oi.mid(6,2).toInt()
        );
        orario oEnd(
            of.mid(0,2).toInt(),
            of.mid(3,2).toInt(),
            of.mid(6,2).toInt()
        );

        auto ev = std::make_shared<eventoLungo>(
            q.value("Id").toInt(),
            dtStart,
            q.value("Priorita").toUInt(),
            q.value("Nome").toString().toStdString(),
            dtEnd,
            q.value("Descrizione").toString().toStdString(),
            oStart,
            oEnd
        );

        m_eventModel->addEvent(ev);
    }

    dateTime d(date.year(), date.month(), date.day(), 0, 0, 0);
    m_dayWeekView->setDate(d);
}

static dateTime chronoToDateTime(const jm::dateTime& tp)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&tt);

    return dateTime(
        tm->tm_year + 1900,
        tm->tm_mon,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec
    );
}

static orario orarioFromOptional(const std::optional<std::string>& s)
{
    if (!s.has_value()) return orario(0,0,0);
    return orarioFromString(*s);
}

void MainWindow::loadEventsFromDb()
{
    if (!m_db.isValid() || !m_db.isOpen())
        return;

    m_eventModel->clear();

    QSqlQuery q(m_db);
    q.prepare("SELECT id, name, start_datetime, end_datetime, type, description, priority "
              "FROM events WHERE user_id = ? ORDER BY start_datetime");
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
        QString desc = q.value(5).toString();
        int priority = q.value(6).toInt();

        // Conversione stringa → dateTime
        dateTime dtStart = qStringToDateTime(start);
        dateTime dtEnd   = qStringToDateTime(end);

        // Conversione dateTime → orario
        orario oStart(dtStart.getSec(), dtStart.getMin(), dtStart.getHour());
        orario oEnd(dtEnd.getSec(), dtEnd.getMin(), dtEnd.getHour());

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

    m_dayWeekView->updateView();
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
}

int ensureUserExists(QSqlDatabase& db, const QString& name, const QString& email, const QString& password)
{
    if (!db.isOpen()) {
        qDebug() << "ensureUserExists: database non aperto";
        return -1;
    }

QSqlQuery q(db);
q.prepare("SELECT Id FROM Persona WHERE Email = \":email\"");
q.bindValue(":email", email);

    if (!q.exec()) {
    qDebug() << "Query:" << q.lastQuery();
    qDebug() << "Errore SQL:" << q.lastError().text();
    return -2;
}


    if (q.next()) {
        int id = q.value(0).toInt();
        QMessageBox::information(nullptr, "Accesso", "Bentornato, " + name + "!");
        return id;
    }

    QSqlQuery ins(db);
    ins.prepare("INSERT INTO persona (Nome, Email, Password) VALUES (\"?\", \"?\", \"?\")");
    ins.bindValue(0, name);
    ins.bindValue(1, email);
    ins.bindValue(2, password);

    if (!ins.exec()) {
        qDebug() << "ensureUserExists: INSERT fallita -" << ins.lastError().text();
        return -3;
    }

    QVariant lastId = ins.lastInsertId();
    if (lastId.isValid()) {
        QMessageBox::information(nullptr, "Registrazione", "Utente creato con successo.");
        return lastId.toInt();
    }

    // fallback
    QSqlQuery q2(db);
    q2.prepare("SELECT Id FROM persona WHERE Email = \"?\"");
    q2.addBindValue(email);
    if (q2.exec() && q2.next()) return q2.value(0).toInt();

    qDebug() << "ensureUserExists: impossibile ottenere lastInsertId e SELECT fallback fallita";
    return -4;
}

