#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractListModel>
#include <vector>
#include <memory>
#include "fileCpp/impegno/impegno.h"

class EventModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit EventModel(QObject* parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        StartRole,
        EndRole,
        PriorityRole,
        DescriptionRole,
        ColorRole,   // <--- AGGIUNGI QUESTO
        IconRole 
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addEvent(const std::shared_ptr<eventoLungo>& ev);
    void clear();

    std::vector<std::shared_ptr<eventoLungo>> eventsForDate(const dateTime& dt) const;

signals:
    void eventsChanged();

private:
    void sortByStart();

    std::vector<std::shared_ptr<eventoLungo>> m_events;
};

#endif // EVENTMODEL_H
