#include "DayWeekView.h"
#include "EventModel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDateTime>
#include <QDebug>

DayWeekView::DayWeekView(QWidget* parent)
    : QWidget(parent),
      m_date(QDate::currentDate())
{
    setMinimumHeight(400);
    setMinimumWidth(600);
}

void DayWeekView::setModel(EventModel* model) {
    m_model = model;
    update();
}

void DayWeekView::setMode(Mode m) {
    m_mode = m;
    update();
}

void DayWeekView::setDate(const QDate& d) {
    m_date = d;
    update();
}

QColor DayWeekView::colorForType(int type) const {
    switch (type) {
    case 1: return QColor("#4CAF50"); // compleanno
    case 2: return QColor("#2196F3"); // riunione
    case 3: return QColor("#FF9800"); // raggruppa
    default: return QColor("#9E9E9E"); // generico
    }
}

QPixmap DayWeekView::iconForType(int type) const {
    if (type == 1) return QPixmap(":/icons/birthday.png");
    if (type == 2) return QPixmap(":/icons/meeting.png");
    if (type == 3) return QPixmap(":/icons/group.png");
    return QPixmap();
}

void DayWeekView::paintEvent(QPaintEvent* /*ev*/) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    int cols = (m_mode == Week) ? 7 : 1;
    int colW = width() / cols;
    int totalMinutes = (m_hourEnd - m_hourStart) * 60;
    int rows = totalMinutes / m_slotMinutes;
    int slotH = (rows > 0) ? (height() / rows) : 20;

    // Draw columns and headers
    for (int c = 0; c < cols; ++c) {
        QRect colRect(c * colW, 0, colW, height());
        p.setPen(QColor("#E0E0E0"));
        p.drawRect(colRect);

        QDate day = m_date.addDays(c);
        p.setPen(Qt::black);
        p.drawText(colRect.adjusted(6, 6, -6, -height() + 30), Qt::AlignLeft | Qt::AlignTop, day.toString("ddd dd MMM"));
    }

    // Draw horizontal time lines and labels
    for (int r = 0; r <= rows; ++r) {
        int y = r * slotH;
        p.setPen(QColor("#F5F5F5"));
        p.drawLine(0, y, width(), y);
        if (r % (60 / m_slotMinutes) == 0) {
            int hour = m_hourStart + (r * m_slotMinutes) / 60;
            p.setPen(Qt::gray);
            p.drawText(2, y + 12, QString("%1:00").arg(hour, 2, 10, QChar('0')));
        }
    }

    // Render events per column/day
    for (int c = 0; c < cols; ++c) {
        QDate day = m_date.addDays(c);
        auto events = m_model ? m_model->eventsForDate(day) : std::vector<std::shared_ptr<eventoLungo>>{};
        auto renders = layoutEvents(events, colW);
        for (const auto& re : renders) {
            QColor bg = colorForType(re.ev->getType());
            p.fillRect(re.rect.translated(c * colW, 0), bg.lighter(120));
            p.setPen(bg.darker());
            p.drawRect(re.rect.translated(c * colW, 0));
            p.setPen(Qt::black);
            QRect textRect = re.rect.translated(c * colW, 0).adjusted(4, 4, -4, -4);
            p.drawText(textRect, Qt::TextWordWrap, QString::fromStdString(re.ev->getNome()));
            QPixmap ic = iconForType(re.ev->getType());
            if (!ic.isNull()) {
                p.drawPixmap(re.rect.right() + c * colW - 20, re.rect.top() + 4, 16, 16, ic);
            }
        }
    }
}

QVector<DayWeekView::RenderEvent> DayWeekView::layoutEvents(const std::vector<std::shared_ptr<eventoLungo>>& events, int width) const {
    QVector<RenderEvent> out;
    int totalMinutes = (m_hourEnd - m_hourStart) * 60;
    int rows = totalMinutes / m_slotMinutes;
    int slotH = (rows > 0) ? (height() / rows) : 20;

    for (const auto& ev : events) {
        // Parse times from domain dateTime strings (best-effort)
        QString sdt = QString::fromStdString(ev->getMomentoInizio().getDateTime());
        QString edt = QString::fromStdString(ev->getMomentoFine().getDateTime());
        QTime s = QTime::fromString(sdt.mid(11, 8), "hh:mm:ss");
        QTime e = QTime::fromString(edt.mid(11, 8), "hh:mm:ss");
        if (!s.isValid()) s = QTime(m_hourStart, 0);
        if (!e.isValid()) e = s.addSecs(3600);

        int startMin = (s.hour() * 60 + s.minute()) - m_hourStart * 60;
        int durMin = s.secsTo(e) / 60;
        if (startMin < 0) startMin = 0;
        if (durMin <= 0) durMin = m_slotMinutes;

        int y = (startMin / m_slotMinutes) * slotH;
        int h = std::max(20, (durMin / m_slotMinutes) * slotH - 2);
        QRect r(4, y + 30, width - 12, h);
        out.push_back({r, ev});
    }

    // TODO: handle overlapping events by splitting width and offsetting horizontally
    return out;
}

void DayWeekView::mousePressEvent(QMouseEvent* ev) {
    // TODO: detect clicks on events and emit signals to open detail dialogs
    QWidget::mousePressEvent(ev);
}
