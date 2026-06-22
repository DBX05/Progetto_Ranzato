#include "DayWeekView.h"
#include "EventModel.h"
#include "util_datetime.h"
#include <QPainter>
#include <QMouseEvent>

DayWeekView::DayWeekView(QWidget* parent)
    : QWidget(parent),
      m_model(nullptr),
      m_mode(Mode::Day)
{
}

void DayWeekView::setModel(EventModel* model)
{
    m_model = model;
    if (m_model) {
        connect(m_model, &EventModel::eventsChanged,
                this, &DayWeekView::updateView);
    }
}

void DayWeekView::setMode(Mode mode)
{
    m_mode = mode;
    updateView();
}

void DayWeekView::setDate(const dateTime& dt)
{
    m_date = dt;
    updateView();
}

void DayWeekView::updateView()
{
    if (!m_model) return;
    m_visibleEvents = m_model->eventsForDate(m_date);
    update();
}

void DayWeekView::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    // griglia oraria semplice
    int h = height();
    int w = width();
    int hours = 24;
    for (int i = 0; i <= hours; ++i) {
        int y = h * i / hours;
        p.setPen(QColor(220, 220, 220));
        p.drawLine(0, y, w, y);
        p.setPen(Qt::black);
        p.drawText(5, y - 2, QString("%1:00").arg(i, 2, 10, QChar('0')));
    }

    // eventi
    for (const auto& ev : m_visibleEvents) {
        QString sdt = QString::fromStdString(ev->getMomentoInizio().getDateTime());
        QString edt = QString::fromStdString(ev->getMomentoFine().getDateTime());

        QDateTime qStart = QDateTime::fromString(sdt, "yyyy-MM-dd HH:mm:ss");
        QDateTime qEnd   = QDateTime::fromString(edt, "yyyy-MM-dd HH:mm:ss");
        if (!qStart.isValid() || !qEnd.isValid()) continue;

        int startMin = qStart.time().hour() * 60 + qStart.time().minute();
        int endMin   = qEnd.time().hour() * 60 + qEnd.time().minute();
        if (endMin <= startMin) endMin = startMin + 30;

        double startRatio = startMin / (24.0 * 60.0);
        double endRatio   = endMin   / (24.0 * 60.0);

        QRect r(80,
                int(startRatio * h),
                w - 100,
                int((endRatio - startRatio) * h));

        p.setBrush(QColor(100, 150, 250, 180));
        p.setPen(Qt::NoPen);
        p.drawRect(r);

        p.setPen(Qt::white);
        p.drawText(r.adjusted(4, 4, -4, -4),
                   Qt::AlignLeft | Qt::AlignTop,
                   QString::fromStdString(ev->getNome()));
    }
}
