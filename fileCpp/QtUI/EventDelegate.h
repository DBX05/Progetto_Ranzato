#ifndef EVENTDELEGATE_H
#define EVENTDELEGATE_H

#include <QStyledItemDelegate>

class EventDelegate : public QStyledItemDelegate
{
public:
    EventDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

#endif
