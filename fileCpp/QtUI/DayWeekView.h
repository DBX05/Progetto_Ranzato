#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <QDate>
#include <QVector>
#include <memory>
#include <fileCpp/impegno/impegno.h>

class EventModel;

class DayWeekView : public QWidget
{
    Q_OBJECT

public:
    enum Mode { Day = 0, Week = 1 };

    explicit DayWeekView(QWidget* parent = nullptr);

    void setModel(EventModel* model);
    void setMode(Mode m);
    void setDate(const QDate& d);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;

private:
    EventModel* m_model = nullptr;
    Mode m_mode = Day;
    QDate m_date;
    int m_hourStart = 7;
    int m_hourEnd = 22;
    int m_slotMinutes = 30;

    QColor colorForType(int type) const;
    QPixmap iconForType(int type) const;

    struct RenderEvent {
        QRect rect;
        std::shared_ptr<eventoLungo> ev;
    };

    QVector<RenderEvent> layoutEvents(const std::vector<std::shared_ptr<eventoLungo>>& events, int width) const;
};

#endif // DAYWEEKVIEW_H
