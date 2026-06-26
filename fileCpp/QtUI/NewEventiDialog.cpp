// NewEventiDialog.cpp
#include "NewEventiDialog.h"
#include "../utils/util_datetime.h"   // <- assicurati che questo header sia nel target
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimeEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDate>
#include <QString>

NewEventDialog::NewEventDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Nuovo evento");
    setMinimumSize(480, 360);
    resize(520, 400);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    auto* title = new QLabel("Compila i dati dell'evento", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    auto* formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setSpacing(8);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Titolo dell'evento");
    formLayout->addRow("Nome evento:", m_nameEdit);

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    formLayout->addRow("Data:", m_dateEdit);

    m_startEdit = new QTimeEdit(QTime::currentTime(), this);
    m_startEdit->setDisplayFormat("HH:mm");
    formLayout->addRow("Ora inizio:", m_startEdit);

    m_endEdit = new QTimeEdit(QTime::currentTime().addSecs(3600), this);
    m_endEdit->setDisplayFormat("HH:mm");
    formLayout->addRow("Ora fine:", m_endEdit);

    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem("Generico", 0);
    m_typeCombo->addItem("Compleanno", 1);
    m_typeCombo->addItem("Riunione", 2);
    m_typeCombo->addItem("Raggruppa", 3);
    formLayout->addRow("Tipo evento:", m_typeCombo);

    layout->addLayout(formLayout);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

std::shared_ptr<evento> NewEventDialog::createEvento() const {
    // Costruisci stringhe "YYYY-MM-DD HH:MM:SS"
    QDate date = m_dateEdit->date();
    QTime s = m_startEdit->time();
    QTime e = m_endEdit->time();

    QString sStr = QString("%1-%2-%3 %4:%5:00")
                       .arg(date.year(), 4, 10, QChar('0'))
                       .arg(date.month(), 2, 10, QChar('0'))
                       .arg(date.day(), 2, 10, QChar('0'))
                       .arg(s.hour(), 2, 10, QChar('0'))
                       .arg(s.minute(), 2, 10, QChar('0'));
    QString eStr = QString("%1-%2-%3 %4:%5:00")
                       .arg(date.year(), 4, 10, QChar('0'))
                       .arg(date.month(), 2, 10, QChar('0'))
                       .arg(date.day(), 2, 10, QChar('0'))
                       .arg(e.hour(), 2, 10, QChar('0'))
                       .arg(e.minute(), 2, 10, QChar('0'));

    // Converte in dateTime (classe di dominio)
    dateTime dtStart = qStringToDateTime(sStr);
    dateTime dtEnd   = qStringToDateTime(eStr);

    // Costruisci orario (adatta l'ordine dei parametri se la tua classe è diversa)
    orario oStart(dtStart.getSec(), dtStart.getMin(), dtStart.getHour());
    orario oEnd(dtEnd.getSec(), dtEnd.getMin(), dtEnd.getHour());

    // Tipo evento selezionato
    int type = m_typeCombo->currentData().toInt();

    // Crea eventoLungo con la firma coerente usata altrove nel progetto
    // Firma ipotetica: eventoLungo(int id, dateTime start, unsigned int priority, std::string name, dateTime end, std::string description, orario startOrario, orario endOrario)
    // Se la tua firma è diversa, adatta i parametri qui sotto.
    auto ev = std::make_shared<eventoLungo>(
        -1,                 // id temporaneo
        dtStart,            // dateTime inizio
        static_cast<unsigned int>(1), // priority default
        m_nameEdit->text().toStdString(), // nome
        dtEnd,              // dateTime fine
        std::string(""),    // descrizione vuota
        oStart,             // orario inizio
        oEnd                // orario fine
        );

    // Se la classe eventoLungo ha metodi per impostare il tipo, chiamali qui:
    // ev->setType(type);

    return ev;
}

QString NewEventDialog::summary() const {
    QString name = m_nameEdit->text();
    QString date = m_dateEdit->date().toString("yyyy-MM-dd");
    QString start = m_startEdit->time().toString("HH:mm");
    QString end = m_endEdit->time().toString("HH:mm");
    QString type = m_typeCombo->currentText();
    return QString("%1\n%2 %3-%4\nTipo: %5").arg(name, date, start, end, type);
}
