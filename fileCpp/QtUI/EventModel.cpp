// EventModel.cpp
#include "EventModel.h"
#include <QVariant>
#include <QColor>
#include <QIcon>
#include <algorithm>
#include <QDateTime>
#include <QDebug>

// Define role IDs for custom data roles
enum EventRoles {
    IdRole = Qt::UserRole + 1,
    NameRole,
    StartRole,
    EndRole,
    PriorityRole,
    DescriptionRole,
    ColorRole,
    IconRole,
    EventTypeRole // Added for event type
};

EventModel::EventModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int EventModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0; // We are a list, no hierarchy
    return static_cast<int>(m_events.size());
}

QVariant EventModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_events.size()))
        return {}; // Return invalid variant if index is out of bounds

    const auto& ev = m_events[index.row()]; // Get the event by row index

    switch (role) {
        case Qt::DisplayRole: {
            return QString::fromStdString(ev->buildUiSummary());
        }

        case Qt::DecorationRole: {
             // Display an icon for the event, based on priority
            int p = ev->getPriorita();
            // Assuming these icons are available in the application's resource system
            if (p == 1) return QIcon(":/icons/low.png");
            if (p == 2) return QIcon(":/icons/medium.png");
            if (p == 3) return QIcon(":/icons/high.png");
            return QIcon(":/icons/default.png"); // Default icon
        }

        case Qt::BackgroundRole: {
            // Set background color based on priority
            int p = ev->getPriorita();
            if (p == 1) return QColor("#E8F5E9");   // Light Green for low priority
            if (p == 2) return QColor("#FFF8E1");   // Light Yellow for medium priority
            if (p == 3) return QColor("#FFEBEE");   // Light Red for high priority
            return QColor("#ECEFF1");               // Light Gray for default
        }

        // --- Custom Roles for detailed data access ---
        case IdRole:
            return ev->getId();

        case NameRole:
            return QString::fromStdString(ev->getNome());

        case StartRole: {
            // Return QDateTime for easier handling in views/delegates
            QDateTime startDateTime = QDateTime::fromString(QString::fromStdString(ev->getMomentoInizio().getDateTime()), Qt::ISODate);
            return startDateTime.isValid() ? startDateTime : QDateTime(); // Return invalid QDateTime if parsing fails
        }
        case EndRole: {
            QDateTime endDateTime = QDateTime::fromString(QString::fromStdString(ev->getMomentoFine().getDateTime()), Qt::ISODate);
            return endDateTime.isValid() ? endDateTime : QDateTime();
        }
        case PriorityRole:
            return static_cast<int>(ev->getPriorita());

        case DescriptionRole:
            return QString::fromStdString(ev->suggestedAction());

        case ColorRole: {
            // Provide a color role, distinct from BackgroundRole, possibly for finer control
            int p = ev->getPriorita();
            if (p == 1) return QColor("#388E3C");   // Dark Green
            if (p == 2) return QColor("#F57F17");   // Dark Yellow
            if (p == 3) return QColor("#C62828");   // Dark Red
            return QColor("#546E7A");               // Dark Gray
        }

        case IconRole: {
            // Explicit IconRole, potentially redundant with DecorationRole but good for clarity
            int p = ev->getPriorita();
            if (p == 1) return QIcon(":/icons/low.png");
            if (p == 2) return QIcon(":/icons/medium.png");
            if (p == 3) return QIcon(":/icons/high.png");
            return QIcon(":/icons/default.png");
        }
        case EventTypeRole:
            return QString::fromStdString(ev->getevName()); // Assuming getEventType() exists

        default:
            return {}; // Return invalid variant for unknown roles
    }
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    // Map the enum roles to byte array names for use in QML or other contexts
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[StartRole] = "start";
    roles[EndRole] = "end";
    roles[PriorityRole] = "priority";
    roles[DescriptionRole] = "description";
    roles[ColorRole] = "color";
    roles[IconRole] = "icon";
    roles[EventTypeRole] = "eventType"; // Added for event type
    return roles;
}

void EventModel::addEvent(const std::shared_ptr<eventoLungo>& ev)
{
    if (!ev) return; // Safety check

    // Use model manipulation functions for proper signal emission
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_events.push_back(ev);
    endInsertRows();

    sortByStart(); // Keep the list sorted by start time
    emit eventsChanged(); // Signal that the events list has been modified
}

void EventModel::clear()
{
    beginResetModel(); // Signal that the entire model is being reset
    m_events.clear();
    endResetModel();
    emit eventsChanged(); // Signal that events have changed (in this case, cleared)
}

std::vector<std::shared_ptr<eventoLungo>> EventModel::eventsForDate(const dateTime& dt) const
{
    std::vector<std::shared_ptr<eventoLungo>> resultEvents;
    QString targetDateString = QString::fromStdString(dt.getDateTime()).mid(0, 10); // Extract YYYY-MM-DD

    for (const auto& ev : m_events) {
        QString eventStartDateString = QString::fromStdString(ev->getMomentoInizio().getDateTime()).mid(0, 10);
        QString eventEndDateString   = QString::fromStdString(ev->getMomentoFine().getDateTime()).mid(0, 10);

        // An event is considered "for" a date if the target date falls within its start and end dates (inclusive)
        if (eventStartDateString <= targetDateString && eventEndDateString >= targetDateString) {
            resultEvents.push_back(ev);
        }
    }
    return resultEvents;
}

void EventModel::sortByStart()
{
    // Sort the internal vector of events based on their start time
    std::sort(m_events.begin(), m_events.end(),
              [](const std::shared_ptr<eventoLungo>& a, const std::shared_ptr<eventoLungo>& b) {
                  // Compare timestamps for accurate sorting
                  return a->getMomentoInizio().toTimestamp() < b->getMomentoInizio().toTimestamp();
              });
    // Note: If sorting affects the displayed order, you might need to emit layoutChanged()
    // emit layoutChanged();
}
