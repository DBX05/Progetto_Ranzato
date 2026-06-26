#include "EventDelegate.h"
#include "EventModel.h"
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QDateTime>

EventDelegate::EventDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QSize EventDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex& idx) const
{
    QString desc = idx.data(EventModel::DescriptionRole).toString().trimmed();
    return QSize(200, desc.isEmpty() ? 90 : 110);
}

void EventDelegate::paint(QPainter* p,
                          const QStyleOptionViewItem& opt,
                          const QModelIndex& idx) const
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing);

    QRect r = opt.rect.adjusted(4, 4, -4, -4);

    // Sfondo selezione o normale
    if (opt.state & QStyle::State_Selected)
        p->fillRect(opt.rect, QColor(220, 235, 255));
    else
        p->fillRect(opt.rect, QColor(250, 250, 250));

    // Card con bordo arrotondato
    QColor cardColor = idx.data(EventModel::ColorRole).value<QColor>();
    p->setBrush(QColor(255, 255, 255));
    p->setPen(QPen(cardColor.lighter(140), 1));
    p->drawRoundedRect(r, 8, 8);

    // Barra laterale colorata
    QRect bar(r.left(), r.top(), 5, r.height());
    p->setBrush(cardColor);
    p->setPen(Qt::NoPen);
    p->drawRoundedRect(bar, 3, 3);

    int left = r.left() + 14;
    int top  = r.top() + 8;
    int right = r.right() - 8;

    // Nome evento (grassetto)
    QString name = idx.data(EventModel::NameRole).toString();
    QFont boldFont("Arial", 10, QFont::Bold);
    p->setFont(boldFont);
    p->setPen(Qt::black);
    p->drawText(QRect(left, top, right - left, 18), Qt::AlignLeft | Qt::AlignVCenter, name);

    // Tipo evento (badge a destra del nome)
    QString type = idx.data(EventModel::EventTypeRole).toString();
    if (!type.isEmpty()) {
        QFont badgeFont("Arial", 8);
        p->setFont(badgeFont);
        QFontMetrics fm(badgeFont);
        int badgeW = fm.horizontalAdvance(type) + 12;
        QRect badgeRect(right - badgeW, top, badgeW, 16);
        p->setBrush(cardColor);
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(badgeRect, 6, 6);
        p->setPen(Qt::white);
        p->drawText(badgeRect, Qt::AlignCenter, type);
    }

    // Data e orario
    QVariant startVar = idx.data(EventModel::StartRole);
    QVariant endVar   = idx.data(EventModel::EndRole);
    QString startStr = startVar.toDateTime().isValid()
        ? startVar.toDateTime().toString("dd/MM/yyyy HH:mm")
        : "--";
    QString endStr = endVar.toDateTime().isValid()
        ? endVar.toDateTime().toString("HH:mm")
        : "--";

    QFont normalFont("Arial", 9);
    p->setFont(normalFont);
    p->setPen(QColor(80, 80, 80));
    p->drawText(QRect(left, top + 22, right - left, 16),
                Qt::AlignLeft | Qt::AlignVCenter,
                QString("🕒 %1  →  %2").arg(startStr, endStr));

    // Priorità
    int pr = idx.data(EventModel::PriorityRole).toInt();
    QString prLabel = (pr == 1) ? "Bassa" : (pr == 2) ? "Media" : (pr == 3) ? "Alta" : "";
    if (!prLabel.isEmpty()) {
        p->setPen(cardColor);
        QFont prFont("Arial", 8, QFont::Bold);
        p->setFont(prFont);
        p->drawText(QRect(left, top + 40, right - left, 14),
                    Qt::AlignLeft | Qt::AlignVCenter,
                    QString("Priorità: %1").arg(prLabel));
    }

    // Descrizione (se presente)
    QString desc = idx.data(EventModel::DescriptionRole).toString().trimmed();
    if (!desc.isEmpty()) {
        QFont descFont("Arial", 8);
        p->setFont(descFont);
        p->setPen(QColor(100, 100, 100));
        QFontMetrics fm(descFont);
        QString elided = fm.elidedText(desc, Qt::ElideRight, right - left);
        p->drawText(QRect(left, top + 56, right - left, 14),
                    Qt::AlignLeft | Qt::AlignVCenter, elided);
    }

    p->restore();
}
