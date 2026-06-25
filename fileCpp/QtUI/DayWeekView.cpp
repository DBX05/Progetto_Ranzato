#include "DayWeekView.h"
#include "EventModel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDate>
#include <QLocale>
#include <../impegno/impegno.h>

DayWeekView::DayWeekView(QWidget* parent)
    : QWidget(parent),
      m_model(nullptr),
      m_mode(Mode::Week),
      m_currentDate(QDate::currentDate()),
      m_selectedDate(QDate::currentDate())
{
}


void DayWeekView::setModel(EventModel* model)
{
    m_model = model;
}

void DayWeekView::setMode(Mode mode)
{
    m_mode = mode;
    update();
}

void DayWeekView::setDate(const dateTime& dt)
{
    m_currentDate = QDate(
        dt.getAnno(),   // <-- corretto
        dt.numMese(),  // <-- corretto
        dt.getDate()     // <-- corretto
    );

    m_selectedDate = m_currentDate;
    update();
}


void DayWeekView::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    switch (m_mode) {
    case Mode::Day:   drawDay(p);   break;
    case Mode::Week:  drawWeek(p);  break;
    case Mode::Month: drawMonth(p); break;
    case Mode::Year:  drawYear(p);  break;
    }
}

bool DayWeekView::hasEventsOn(const QDate& d) const
{
    if (!m_model) return false;

    int rows = m_model->rowCount();
    for (int i = 0; i < rows; ++i) {
        QModelIndex idx = m_model->index(i, 0);

        QString startStr = m_model->data(idx, EventModel::StartRole).toString();
        QString endStr   = m_model->data(idx, EventModel::EndRole).toString();

        QDate ds = QDate::fromString(startStr.mid(0,10), "yyyy-MM-dd");
        QDate de = QDate::fromString(endStr.mid(0,10), "yyyy-MM-dd");

        if (d >= ds && d <= de)
            return true;
    }
    return false;
}

void DayWeekView::drawMonth(QPainter& p)
{
    QDate base = m_currentDate.isValid() ? m_currentDate : QDate::currentDate();
    QDate first(base.year(), base.month(), 1);

    int startCol = first.dayOfWeek() - 1;   // Lunedì=0, Domenica=6
    int days = first.daysInMonth();

    int cellW = width() / 7;
    int cellH = height() / 6;

    p.setFont(QFont("Arial", 10));

    int day = 1;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {

            int x = col * cellW;
            int y = row * cellH;
            QRect cellRect(x, y, cellW, cellH);

            p.setPen(Qt::black);
            p.drawRect(cellRect);

            if (row == 0 && col < startCol) continue;
            if (day > days) continue;

            QDate current(base.year(), base.month(), day);

            if (hasEventsOn(current)) {
                p.fillRect(cellRect.adjusted(1,1,-1,-1), QColor(220,240,255));
            }

            if (current == m_selectedDate) {
                p.fillRect(cellRect.adjusted(1,1,-1,-1), QColor(200,220,255));
                p.setPen(Qt::blue);
            } else {
                p.setPen(Qt::black);
            }

            p.drawText(x + 5, y + 15, QString::number(day));
            day++;

            auto evs = eventsFor(current);
int offset = 30;

p.setFont(QFont("Arial", 7));

for (auto& ev : evs) {
    QString name = QString::fromStdString(ev->getNome());
    QString desc = QString::fromStdString(ev->getDescrizione());

    QString line = name;
    if (!desc.isEmpty())
        line += " - " + desc;

    p.drawText(x + 5, y + offset, line.left(25)); // evita overflow
    offset += 12;
}

        }
    }
}

void DayWeekView::drawYear(QPainter& p)
{
    int cellW = width() / 4;
    int cellH = height() / 3;

    p.setFont(QFont("Arial", 9));

    QDate base = m_currentDate.isValid() ? m_currentDate : QDate::currentDate();
    int year = base.year();

    for (int m = 1; m <= 12; m++) {

        int row = (m - 1) / 4;
        int col = (m - 1) % 4;

        int x0 = col * cellW;
        int y0 = row * cellH;

        QRect outerRect(x0, y0, cellW, cellH);
        p.setPen(Qt::black);
        p.drawRect(outerRect);
        p.drawText(x0 + 5, y0 + 15, QLocale().monthName(m));


        QDate first(year, m, 1);
        int startCol = first.dayOfWeek() - 1;
        int days = first.daysInMonth();

        int miniW = cellW / 7;
        int miniH = (cellH - 20) / 6;

        int day = 1;
        for (int r = 0; r < 6; r++) {
            for (int c = 0; c < 7; c++) {

                int xx = x0 + c * miniW;
                int yy = y0 + 20 + r * miniH;

                QRect cellRect(xx, yy, miniW, miniH);
                p.drawRect(cellRect);

                if (r == 0 && c < startCol) continue;
                if (day > days) continue;

                QDate current(year, m, day);

                if (hasEventsOn(current)) {
                    p.fillRect(cellRect.adjusted(1,1,-1,-1), QColor(220,240,255));
                }

                if (current == m_selectedDate) {
                    p.fillRect(cellRect.adjusted(1,1,-1,-1), QColor(200,220,255));
                    p.setPen(Qt::blue);
                } else {
                    p.setPen(Qt::black);
                }

                p.drawText(xx + 2, yy + 12, QString::number(day));
                day++;
            }
        }
    }
}

void DayWeekView::mousePressEvent(QMouseEvent* event)
{
    if (m_mode != Mode::Month) {
        QWidget::mousePressEvent(event);
        return;
    }

    int cellW = width() / 7;
    int cellH = height() / 6;

    int col = event->pos().x() / cellW;
    int row = event->pos().y() / cellH;

    QDate base = m_currentDate.isValid() ? m_currentDate : QDate::currentDate();

    // CORRETTO: mese corrente, NON month()-1
    QDate first(base.year(), base.month(), 1);

    int startCol = first.dayOfWeek() - 1;
    int days = first.daysInMonth();

    int index = row * 7 + col;
    int day = index - startCol + 1;

    if (day < 1 || day > days)
        return;

    m_selectedDate = QDate(base.year(), base.month(), day);

    emit daySelected(m_selectedDate);
    update();
}


std::vector<std::shared_ptr<eventoLungo>> DayWeekView::eventsFor(const QDate& d) const
{
    if (!m_model) return {};

    dateTime dt(d.year(), d.month(), d.day(), 0, 0, 0);
    return m_model->eventsForDate(dt);
}


void DayWeekView::drawDay(QPainter& p)
{
    QDate d = m_selectedDate.isValid() ? m_selectedDate : QDate::currentDate();

    // intestazione
    p.setFont(QFont("Arial", 14, QFont::Bold));
    p.drawText(20, 30, d.toString("dddd dd MMMM yyyy"));

    int top = 50;
    int left = 60;
    int hourHeight = (height() - top - 20) / (m_hourEnd - m_hourStart);
    int w = width() - left - 20;

    // griglia oraria
    p.setFont(QFont("Arial", 10));
    for (int h = m_hourStart; h <= m_hourEnd; ++h) {
        int y = top + (h - m_hourStart) * hourHeight;

        p.setPen(QColor(200,200,200));
        p.drawLine(left, y, width() - 10, y);

        p.setPen(Qt::black);
        p.drawText(10, y + 5, QString("%1:00").arg(h, 2, 10, QChar('0')));
    }

    // eventi del giorno
    auto evs = eventsFor(d);

    for (auto& ev : evs) {
        auto start = ev->getMomentoInizio();
        auto end   = ev->getMomentoFine();

        int sh = start.getSec();
        int eh = end.getSec();
        int sm = start.getMin();
        int em = end.getMin();

        double startPos = (sh - m_hourStart) * hourHeight + (sm / 60.0) * hourHeight;
        double endPos   = (eh - m_hourStart) * hourHeight + (em / 60.0) * hourHeight;

        QRect r(left + 5, top + startPos, w - 10, endPos - startPos);

        p.fillRect(r, QColor(180, 210, 255));
        p.setPen(Qt::blue);
        p.drawRect(r);

        p.setPen(Qt::black);
        QString text = QString::fromStdString(ev->getNome()) +
               "\n" +
               QString::fromStdString(ev->getDescrizione());

        p.drawText(r.adjusted(5, 5, -5, -5),
                    Qt::AlignLeft | Qt::AlignTop,
                    text);

    }
}


void DayWeekView::drawWeek(QPainter& p)
{
    QDate base = m_selectedDate.isValid() ? m_selectedDate : QDate::currentDate();
    QDate monday = base.addDays(-(base.dayOfWeek() - 1));

    int top = 40;
    int left = 60;

    int dayWidth = (width() - left - 20) / 7;
    int hourHeight = (height() - top - 20) / (m_hourEnd - m_hourStart);

    // intestazione
    p.setFont(QFont("Arial", 14, QFont::Bold));
    p.drawText(20, 25, QString("Settimana %1 - %2")
               .arg(monday.toString("dd MMM"))
               .arg(monday.addDays(6).toString("dd MMM yyyy")));

    // intestazioni giorni
    p.setFont(QFont("Arial", 10, QFont::Bold));
    for (int i = 0; i < 7; ++i) {
        QDate d = monday.addDays(i);
        int x = left + i * dayWidth;

        if (d == m_selectedDate) {
            p.fillRect(QRect(x, top, dayWidth, 20), QColor(200,220,255));
        }

        p.setPen(Qt::black);
        p.drawText(x + 5, top + 15, d.toString("ddd dd"));
    }

    // griglia oraria
    for (int h = m_hourStart; h <= m_hourEnd; ++h) {
        int y = top + 20 + (h - m_hourStart) * hourHeight;

        p.setPen(QColor(200,200,200));
        p.drawLine(left, y, width() - 10, y);

        p.setPen(Qt::black);
        p.drawText(10, y + 5, QString("%1:00").arg(h, 2, 10, QChar('0')));
    }

    // eventi
    for (int i = 0; i < 7; ++i) {
        QDate d = monday.addDays(i);
        auto evs = eventsFor(d);

        int x = left + i * dayWidth;

        for (auto& ev : evs) {
            auto start = ev->getMomentoInizio();
            auto end   = ev->getMomentoFine();

            int sh = start.getSec();
            int eh = end.getSec();
            int sm = start.getMin();
            int em = end.getMin();

            double startPos = (sh - m_hourStart) * hourHeight + (sm / 60.0) * hourHeight;
            double endPos   = (eh - m_hourStart) * hourHeight + (em / 60.0) * hourHeight;

            QRect r(x + 5, top + 20 + startPos, dayWidth - 10, endPos - startPos);

            p.fillRect(r, QColor(180, 210, 255));
            p.setPen(Qt::blue);
            p.drawRect(r);

            p.setPen(Qt::black);
            QString text = QString::fromStdString(ev->getNome()) +
               "\n" +
               QString::fromStdString(ev->getDescrizione());

            p.drawText(r.adjusted(5, 5, -5, -5),
                        Qt::AlignLeft | Qt::AlignTop,
                        text);

        }
    }
}

