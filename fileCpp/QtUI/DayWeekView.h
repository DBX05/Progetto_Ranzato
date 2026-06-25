#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <QDate>
#include "../dataora/dataora.h"
#include <../impegno/impegno.h>

class EventModel;

class DayWeekView : public QWidget
{
    Q_OBJECT
public:
    enum class Mode { Day, Week, Month, Year };
    int m_hourStart = 0;   // ora di inizio giornata
    int m_hourEnd   = 23;  // ora di fine giornata

    Mode mode() const { return m_mode; }
    QDate selectedDate() const { return m_selectedDate; }
    explicit DayWeekView(QWidget* parent = nullptr);

    void setModel(EventModel* model);
    void setMode(Mode mode);
    void setDate(const dateTime& dt);   // la firma resta così per compatibilità

signals:
    void daySelected(const QDate& date);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    Mode m_mode;
    void drawDay(QPainter& p);
    void drawWeek(QPainter& p);
    void drawMonth(QPainter& p);
    void drawYear(QPainter& p);
    std::vector<std::shared_ptr<eventoLungo>> eventsFor(const QDate& d) const;

    bool hasEventsOn(const QDate& d) const;

    EventModel* m_model;
    QDate m_currentDate;     // <-- QDate, non più dateTime
    QDate m_selectedDate;
};

#endif
