// DayWeekView.cpp
#include "DayWeekView.h"
#include "EventModel.h"
#include "EventDelegate.h"

#include <QCalendarWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>

// ─────────────────────────────────────────────────────────────────
// EventDateFilter
// ─────────────────────────────────────────────────────────────────
EventDateFilter::EventDateFilter(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_date(QDate::currentDate())
{}

void EventDateFilter::setFilterDate(const QDate& date)
{
    m_date = date;
    beginFilterChange();
    endFilterChange();
}

void EventDateFilter::refresh()
{
    beginFilterChange();
    endFilterChange();
}

bool EventDateFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (!m_date.isValid()) return true;

    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    QDateTime startDt = idx.data(EventModel::StartRole).toDateTime();
    QDateTime endDt   = idx.data(EventModel::EndRole).toDateTime();

    if (!startDt.isValid() || !endDt.isValid()) return false;

    return startDt.date() <= m_date && m_date <= endDt.date();
}

// ─────────────────────────────────────────────────────────────────
// DayWeekView
// ─────────────────────────────────────────────────────────────────
DayWeekView::DayWeekView(QWidget* parent)
    : QWidget(parent)
{
    m_filter = new EventDateFilter(this);

    m_listView = new QListView(this);
    m_listView->setModel(m_filter);
    m_listView->setItemDelegate(new EventDelegate(this));
    m_listView->setSpacing(4);
    m_listView->setSelectionMode(QAbstractItemView::NoSelection);
    m_listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listView->setFrameShape(QFrame::NoFrame);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_listView);
    setLayout(layout);
}

void DayWeekView::setModel(EventModel* model)
{
    if (m_model == model) return;

    if (m_model)
        QObject::disconnect(m_model, nullptr, this, nullptr);

    m_model = model;
    m_filter->setSourceModel(m_model);

    if (m_model) {
        QObject::connect(m_model, &EventModel::eventsChanged, this, [this]() {
            m_filter->refresh();
        });
    }
}

void DayWeekView::setCalendarWidget(QCalendarWidget* calendar)
{
    if (m_calendar == calendar) return;

    if (m_calendar)
        QObject::disconnect(m_calendar, nullptr, this, nullptr);

    m_calendar = calendar;

    if (m_calendar) {
        QObject::connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
            if (m_calendar)
                updateLabelEventsForDate(m_calendar->selectedDate());
        });
        updateLabelEventsForDate(m_calendar->selectedDate());
    }
}

void DayWeekView::updateLabelEventsForDate(const QDate& date)
{
    QDate d = date.isValid() ? date
            : (m_calendar ? m_calendar->selectedDate() : QDate::currentDate());
    m_filter->setFilterDate(d);
}
