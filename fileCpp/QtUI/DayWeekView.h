#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <vector>
#include <memory>
#include "fileCpp/dataora/dataora.h"
#include "fileCpp/impegno/impegno.h"

class EventModel;

class DayWeekView : public QWidget
{
    Q_OBJECT
public:
    enum class Mode { Day, Week };

    explicit DayWeekView(QWidget* parent = nullptr);

    void setModel(EventModel* model);
    void setMode(Mode mode);
    void setDate(const dateTime& dt);

public slots:
    void updateView();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    EventModel* m_model;
    Mode m_mode;
    dateTime m_date;

    std::vector<std::shared_ptr<eventoLungo>> m_visibleEvents;
};

#endif // DAYWEEKVIEW_H
