#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <QDate>
#include <QDateTime>
#include <memory>

class EventModel;
class QCalendarWidget;
class QLabel;
class QScrollArea;

class eventoLungo;
namespace dataora { class dateTime; }

class DayWeekView : public QWidget
{
    Q_OBJECT
public:
    explicit DayWeekView(QWidget* parent = nullptr);
    ~DayWeekView() override = default;

    void setModel(EventModel* model);
    void setCalendarWidget(QCalendarWidget* calendar);

    // Aggiorna la label con gli eventi del giorno specificato
    Q_SLOT void updateLabelEventsForDate(const QDate& date);

private:
    static bool parseDateTimeString(const QString& s, QDateTime& out);
    QString buildEventsHtmlForDate(const QDate& date) const;

    EventModel* m_model = nullptr;
    QCalendarWidget* m_calendar = nullptr;
    QLabel* m_labelEvents = nullptr;
    QScrollArea* m_scrollArea = nullptr;
};

#endif // DAYWEEKVIEW_H
