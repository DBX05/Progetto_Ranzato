#include "EventDelegate.h"
#include "EventModel.h"
#include <QPainter>
#include <QApplication>

EventDelegate::EventDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}


QSize EventDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    return QSize(200, 60); // altezza riga
}

void EventDelegate::paint(QPainter* p,
                          const QStyleOptionViewItem& opt,
                          const QModelIndex& idx) const
{
    p->save();

    QRect r = opt.rect;

    // sfondo selezione
    if (opt.state & QStyle::State_Selected)
        p->fillRect(r, QColor(200,220,255));
    else
        p->fillRect(r, QColor(245,245,245));

    // colore evento
    QColor color = idx.data(EventModel::ColorRole).value<QColor>();
    p->setBrush(color);
    p->setPen(Qt::NoPen);
    p->drawRoundedRect(r.adjusted(5,5,-5,-5), 8, 8);

    // icona
    QIcon icon = idx.data(EventModel::IconRole).value<QIcon>();
    QRect iconRect = r.adjusted(10,10,-10,-10);
    iconRect.setWidth(32);
    iconRect.setHeight(32);
    icon.paint(p, iconRect);

    // testo
    QString name = idx.data(EventModel::NameRole).toString();
    QString start = idx.data(EventModel::StartRole).toString().mid(11,5);
    QString end   = idx.data(EventModel::EndRole).toString().mid(11,5);

    p->setPen(Qt::black);
    p->setFont(QFont("Arial", 10, QFont::Bold));
    p->drawText(r.adjusted(55, 8, -10, -30), name);

    p->setFont(QFont("Arial", 9));
    p->drawText(r.adjusted(55, 28, -10, -10),
                QString("%1 - %2").arg(start, end));

    p->restore();
}
