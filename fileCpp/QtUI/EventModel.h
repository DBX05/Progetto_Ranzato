#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractListModel>
#include <QDate>
#include <vector>
#include <memory>

// Domain headers expected to exist in include path
#include <fileCpp/impegno/impegno.h>   // defines eventoLungo and derived types
#include <fileCpp/dataora/dataora.h>  // defines dateTime, orario

class EventModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        StartRole,
        EndRole,
        TypeRole,
        DescriptionRole,
        PriorityRole
    };

    explicit EventModel(QObject* parent = nullptr);

    // QAbstractItemModel overrides
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model API
    void addEvent(std::shared_ptr<eventoLungo> ev);
    std::vector<std::shared_ptr<eventoLungo>> eventsForDate(const QDate& date) const;
    const std::vector<std::shared_ptr<eventoLungo>>& allEvents() const { return m_events; }

private:
    std::vector<std::shared_ptr<eventoLungo>> m_events;
};

#endif // EVENTMODEL_H
