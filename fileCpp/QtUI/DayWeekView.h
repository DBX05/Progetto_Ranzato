#ifndef DAYWEEKVIEW_H
#define DAYWEEKVIEW_H

#include <QWidget>
#include <QDate>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <memory>

class EventModel;
class QCalendarWidget;
class QListView;
class EventDelegate;

// Proxy che filtra le righe del modello tenendo solo gli eventi
// che ricadono nella data selezionata
class EventDateFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EventDateFilter(QObject* parent = nullptr);
    void setFilterDate(const QDate& date);
    void refresh();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QDate m_date;
};

class DayWeekView : public QWidget
{
    Q_OBJECT
public:
    explicit DayWeekView(QWidget* parent = nullptr);
    ~DayWeekView() override = default;

    void setModel(EventModel* model);
    void setCalendarWidget(QCalendarWidget* calendar);

    Q_SLOT void updateLabelEventsForDate(const QDate& date);

private:
    EventModel*      m_model    = nullptr;
    QCalendarWidget* m_calendar = nullptr;
    QListView*       m_listView = nullptr;
    EventDateFilter* m_filter   = nullptr;
};

#endif // DAYWEEKVIEW_H
