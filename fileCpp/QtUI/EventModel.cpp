#include "EventModel.h"
#include <QVariant>
#include <algorithm>

EventModel::EventModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int EventModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_events.size());
}

QVariant EventModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= (int)m_events.size())
        return {};

    const auto& ev = m_events[index.row()];

    switch (role) {
    case IdRole:          return ev->getId();
    case NameRole:        return QString::fromStdString(ev->getNome());
    case StartRole:       return QString::fromStdString(ev->getMomentoInizio().getDateTime());
    case EndRole:         return QString::fromStdString(ev->getMomentoFine().getDateTime());
    case PriorityRole:    return (int)ev->getPriorita();
    case DescriptionRole: return QString::fromStdString(ev->getDescrizione());
    default:              return {};
    }
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]          = "id";
    roles[NameRole]        = "name";
    roles[StartRole]       = "start";
    roles[EndRole]         = "end";
    roles[PriorityRole]    = "priority";
    roles[DescriptionRole] = "description";
    return roles;
}

void EventModel::addEvent(const std::shared_ptr<eventoLungo>& ev)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_events.push_back(ev);
    endInsertRows();

    sortByStart();
    emit eventsChanged();
}

void EventModel::clear()
{
    beginResetModel();
    m_events.clear();
    endResetModel();
    emit eventsChanged();
}

std::vector<std::shared_ptr<eventoLungo>> EventModel::eventsForDate(const dateTime& dt) const
{
    std::vector<std::shared_ptr<eventoLungo>> res;
    QString target = QString::fromStdString(dt.getDateTime()).mid(0, 10); // "YYYY-MM-DD"

    for (const auto& ev : m_events) {
        QString di = QString::fromStdString(ev->getMomentoInizio().getDateTime()).mid(0, 10);
        QString df = QString::fromStdString(ev->getMomentoFine().getDateTime()).mid(0, 10);
        if (di <= target && df >= target)
            res.push_back(ev);
    }
    return res;
}

void EventModel::sortByStart()
{
    std::sort(m_events.begin(), m_events.end(),
              [](const std::shared_ptr<eventoLungo>& a,
                 const std::shared_ptr<eventoLungo>& b) {
                  return a->getMomentoInizio().toTimestamp() <
                         b->getMomentoInizio().toTimestamp();
              });
}
