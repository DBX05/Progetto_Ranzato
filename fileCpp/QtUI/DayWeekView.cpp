// DayWeekView.cpp
#include "DayWeekView.h"
#include "EventModel.h"

// Includi gli header concreti che definiscono i tipi nel tuo progetto.
// Adatta i percorsi se i file sono in cartelle diverse.
#include "../impegno/impegno.h"   // definisce impegno::eventoLungo
#include "dataora.h"       // definisce dataora::dateTime

#include <QCalendarWidget>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>

DayWeekView::DayWeekView(QWidget* parent)
    : QWidget(parent),
      m_model(nullptr),
      m_calendar(nullptr)
{
    m_labelEvents = new QLabel(this);
    m_labelEvents->setTextFormat(Qt::RichText);
    m_labelEvents->setWordWrap(true);
    m_labelEvents->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_labelEvents);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6,6,6,6);
    layout->addWidget(m_scrollArea);
    setLayout(layout);
}

void DayWeekView::setModel(EventModel* model)
{
    if (m_model == model) return;

    if (m_model) {
        QObject::disconnect(m_model, nullptr, this, nullptr);
    }

    m_model = model;

    if (!m_model) {
        updateLabelEventsForDate(m_calendar ? m_calendar->selectedDate() : QDate::currentDate());
        return;
    }

    // --- Sostituisci 'eventsChanged' con il nome reale del segnale del tuo EventModel ---
    QObject::connect(m_model, &EventModel::eventsChanged, this, [this]() {
        if (m_calendar) updateLabelEventsForDate(m_calendar->selectedDate());
    });

    if (m_calendar) updateLabelEventsForDate(m_calendar->selectedDate());
}


void DayWeekView::setCalendarWidget(QCalendarWidget* calendar)
{
    if (m_calendar == calendar) return;

    if (m_calendar) {
        QObject::disconnect(m_calendar, nullptr, this, nullptr);
    }

    m_calendar = calendar;

    if (m_calendar) {
        QObject::connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
            if (!m_calendar) return;
            updateLabelEventsForDate(m_calendar->selectedDate());
        });

        // Aggiorna subito
        updateLabelEventsForDate(m_calendar->selectedDate());
    }
}

bool DayWeekView::parseDateTimeString(const QString& s, QDateTime& out)
{
    out = QDateTime();
    if (s.isEmpty()) return false;

    QString trimmed = s.left(19); // yyyy-MM-dd HH:mm:ss
    out = QDateTime::fromString(trimmed, "yyyy-MM-dd HH:mm:ss");
    if (out.isValid()) return true;

    out = QDateTime::fromString(trimmed, Qt::ISODate);
    if (out.isValid()) return true;

    out = QDateTime::fromString(trimmed);
    return out.isValid();
}

QString DayWeekView::buildEventsHtmlForDate(const QDate& date) const
{
    if (!m_model) return QString("<i>Nessun modello impostato</i>");

    // Costruisco un dataora::dateTime per interrogare il modello.
    // Il costruttore interno usa mese 0-11 e giorno del mese separato dal giorno della settimana.
    dateTime queryDt(date.year(), date.month() - 1, date.day(), 0, 0, 0, 0);

    auto events = m_model->eventsForDate(queryDt);
    if (events.empty()) {
        return QString("<i>Nessun evento per %1</i>").arg(date.toString("dddd dd MMMM yyyy"));
    }

    QString html;
    html += QString(
        "<div style='font-family:Arial, sans-serif; color:#222; line-height:1.3;'>"
        "<div style='font-size:13px; font-weight:bold; margin-bottom:8px;'>📅 %1</div>"
        "<div style='border-bottom:1px solid #d9d9d9; margin-bottom:8px;'></div>"
    ).arg(date.toString("dddd dd MMMM yyyy"));

    for (const auto& evPtr : events) {
        if (!evPtr) continue;

        QString name = QString::fromStdString(evPtr->getNome()).toHtmlEscaped();
        QString typeName = QString::fromStdString(evPtr->getevName());

        QString startStr = QString::fromStdString(evPtr->getMomentoInizio().getDateTime());
        QString endStr   = QString::fromStdString(evPtr->getMomentoFine().getDateTime());

        QDateTime startDt, endDt;
        bool okStart = parseDateTimeString(startStr, startDt);
        bool okEnd   = parseDateTimeString(endStr, endDt);

        QString timeText;
        if (okStart && okEnd)
            timeText = QString("\xF0\x9F\x95\x92 %1  \xE2\x86\x92  %2").arg(startDt.toString("dd/MM/yyyy HH:mm"), endDt.toString("HH:mm"));
        else if (okStart)
            timeText = QString("\xF0\x9F\x95\x92 %1").arg(startDt.toString("dd/MM/yyyy HH:mm"));
        else
            timeText = "\xF0\x9F\x95\x92 orario non disponibile";

        int pr = evPtr->getPriorita();
        QString color = (pr == 1) ? "#388E3C" :
                        (pr == 2) ? "#F57F17" :
                        (pr == 3) ? "#C62828" : "#546E7A";
        QString prLabel = (pr == 1) ? "Bassa" :
                          (pr == 2) ? "Media" :
                          (pr == 3) ? "Alta"  : "";

        QString desc = QString::fromStdString(evPtr->getDescrizione()).trimmed().toHtmlEscaped();

        // badge tipo (stessa logica del delegate)
        QString badgeHtml;
        if (!typeName.isEmpty()) {
            badgeHtml = QString(
                "<span style='background:%1; color:white; font-size:10px; font-weight:bold;"
                " padding:1px 6px; border-radius:6px;'>%2</span>"
            ).arg(color, typeName);
        }

        // priorità
        QString prHtml;
        if (!prLabel.isEmpty()) {
            prHtml = QString(
                "<div style='font-size:10px; color:%1; font-weight:bold; margin-top:3px;'>Priorit&agrave;: %2</div>"
            ).arg(color, prLabel);
        }

        // descrizione
        QString descHtml;
        if (!desc.isEmpty()) {
            descHtml = QString(
                "<div style='font-size:10px; color:#666; margin-top:3px;'>%1</div>"
            ).arg(desc);
        }

        html += QString(
            "<div style='background:white; border:1px solid #e0e0e0;"
            " border-left:4px solid %1; border-radius:8px;"
            " padding:8px 10px; margin-bottom:8px;'>"
            "  <table width='100%' cellspacing='0' cellpadding='0'><tr>"
            "    <td><span style='font-weight:bold; font-size:12px; color:#222;'>%2</span></td>"
            "    <td align='right'>%3</td>"
            "  </tr></table>"
            "  <div style='font-size:10px; color:#555; margin-top:3px;'>%4</div>"
            "  %5%6"
            "</div>"
        ).arg(color, name, badgeHtml, timeText, prHtml, descHtml);
    }

    html += "</div>";
    return html;
}

void DayWeekView::updateLabelEventsForDate(const QDate& date)
{
    if (!m_labelEvents) return;

    // Se la chiamata arriva via SIGNAL/SLOT con QDate vuota, preferiamo la data selezionata del calendario
    QDate d = date.isValid() ? date : (m_calendar ? m_calendar->selectedDate() : QDate::currentDate());

    QString html = buildEventsHtmlForDate(d);
    m_labelEvents->setText(html);
    m_labelEvents->adjustSize();
}
