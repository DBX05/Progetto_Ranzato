#include "NewEventiDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimeEdit>
#include <QDialogButtonBox>

NewEventDialog::NewEventDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Nuovo Evento");
    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Nome evento:"));
    m_nameEdit = new QLineEdit(this);
    layout->addWidget(m_nameEdit);

    layout->addWidget(new QLabel("Ora inizio:"));
    m_startEdit = new QTimeEdit(QTime::currentTime(), this);
    m_startEdit->setDisplayFormat("HH:mm");
    layout->addWidget(m_startEdit);

    layout->addWidget(new QLabel("Ora fine:"));
    m_endEdit = new QTimeEdit(QTime::currentTime().addSecs(3600), this);
    m_endEdit->setDisplayFormat("HH:mm");
    layout->addWidget(m_endEdit);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

std::shared_ptr<evento> NewEventDialog::createEvento() const {
    // Build dateTime for now (date part uses system date)
    dateTime now; // uses system date/time from domain class
    // Create orario from QTime
    QTime s = m_startEdit->time();
    QTime e = m_endEdit->time();

    orario inizio(s.second(), s.minute(), s.hour());
    orario fine(e.second(), e.minute(), e.hour());

    // Use id = -1 (or 0) and priority = 1 as defaults
    auto ev = std::make_shared<evento>(
        -1,
        now,
        static_cast<unsigned int>(1),
        m_nameEdit->text().toStdString(),
        inizio,
        fine
    );
    return ev;
}
