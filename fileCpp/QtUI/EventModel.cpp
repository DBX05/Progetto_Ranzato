#include "EventModel.h"
#include <QDateTime>
#include <algorithm>

EventModel::EventModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int EventModel::rowCount(const QModelIndex& /*parent*/) const {
    return static_cast<int>(m_events.size());
}

QVariant EventModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    int row = index.row();
    if (row < 0 || row >= static_cast<int>(m_events.size())) return {};

    auto ev = m_events[static_cast<size_t>(row)];
    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return QString::fromStdString(ev->getNome());
    case StartRole:
        return QString::fromStdString(ev->getMomentoInizio().getDateTime());
    case EndRole:
        return QString::fromStdString(ev->getMomentoFine().getDateTime());
    case TypeRole:
        return ev->getType();
    case DescriptionRole:
        return QString::fromStdString(ev->getDescrizione());
    case PriorityRole:
        return QVariant::fromValue(static_cast<int>(ev->getPriorita()));
    case IdRole:
        return QVariant::fromValue(ev->getId());
    default:
        return {};
    }
}

QHash<int, QByteArray> EventModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[StartRole] = "start";
    roles[EndRole] = "end";
    roles[TypeRole] = "type";
    roles[DescriptionRole] = "description";
    roles[PriorityRole] = "priority";
    return roles;
}

void EventModel::addEvent(std::shared_ptr<eventoLungo> ev) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_events.push_back(ev);
    endInsertRows();
}

std::vector<std::shared_ptr<eventoLungo>> EventModel::eventsForDate(const QDate& date) const {
    std::vector<std::shared_ptr<eventoLungo>> out;
    for (const auto& ev : m_events) {
        // Convert domain dateTime to QDate for comparison
        // Expect getDateTime() returns "DD/MM/YYYY HH:MM:SS" or similar; try to parse robustly
        std::string dtStart = ev->getMomentoInizio().getDateTime();
        std::string dtEnd = ev->getMomentoFine().getDateTime();

        // Attempt to parse YYYY-MM-DD or DD/MM/YYYY patterns
        QDate qStart = QDate::fromString(QString::fromStdString(dtStart).mid(0,10), "yyyy-MM-dd");
        if (!qStart.isValid()) qStart = QDate::fromString(QString::fromStdString(dtStart).mid(0,10), "dd/MM/yyyy");
        QDate qEnd = QDate::fromString(QString::fromStdString(dtEnd).mid(0,10), "yyyy-MM-dd");
        if (!qEnd.isValid()) qEnd = QDate::fromString(QString::fromStdString(dtEnd).mid(0,10), "dd/MM/yyyy");

        if (!qStart.isValid() || !qEnd.isValid()) {
            // fallback: use system date of start
            qStart = QDate::currentDate();
            qEnd = qStart;
        }

        if ( (date >= qStart && date <= qEnd) ) {
            out.push_back(ev);
        }
    }
    // Optional: sort by start time
    std::sort(out.begin(), out.end(), [](const std::shared_ptr<eventoLungo>& a, const std::shared_ptr<eventoLungo>& b){
        return a->getMomentoInizio().getTimestamp() < b->getMomentoInizio().getTimestamp();
    });
    return out;
}
