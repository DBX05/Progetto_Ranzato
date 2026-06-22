#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <QDate>
#include "../dataora/dataora.h"

class EventModel;

class DayWeekView : public QWidget
{
    Q_OBJECT
public:
    enum class Mode { Day, Week, Month, Year };

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
    void drawDay(QPainter& p);
    void drawWeek(QPainter& p);
    void drawMonth(QPainter& p);
    void drawYear(QPainter& p);

    bool hasEventsOn(const QDate& d) const;

    EventModel* m_model;
    Mode m_mode;
    QDate m_currentDate;     // <-- QDate, non più dateTime
    QDate m_selectedDate;
};

#endif
