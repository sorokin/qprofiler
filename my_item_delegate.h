#ifndef my_item_delegate_H
#define my_item_delegate_H

#include <QItemDelegate>

struct my_item_delegate : QItemDelegate
{
    Q_OBJECT

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
};

#endif // my_item_delegate_H
