#include "myitemdelegate.h"
#include "call_tree_column.h"

namespace
{
    QString friendly_percentage(double perc)
    {
        int integer = (int)perc;
        int frac1 = int(perc * 10.) % 10;
        int frac2 = int(perc * 100.) % 10;
        return QString("%0.%1%2").arg(integer).arg(frac1).arg(frac2);
    }
}

void MyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == CALL_TREE_PERCENTAGE_COLUMN)
    {
        QStyleOptionViewItemV2 opt = QItemDelegate::setOptions(index, option);
        QItemDelegate::drawBackground(painter, opt, index);
        QItemDelegate::drawDisplay(painter, opt, option.rect, friendly_percentage(index.data().toDouble()));
    }
    else
        QItemDelegate::paint(painter, option, index);
}

QSize MyItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index);
}
