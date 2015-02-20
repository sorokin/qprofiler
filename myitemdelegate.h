#ifndef MYITEMDELEGATE_H
#define MYITEMDELEGATE_H

#include <QItemDelegate>

struct MyItemDelegate : QItemDelegate
{
    Q_OBJECT

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
};

#endif // MYITEMDELEGATE_H
