#include "my_item.h"
#include "call_tree_column.h"
#include "my_context.h"

MyItem::MyItem(MyContext *ctx, const QString &function_name)
    : ctx(ctx)
    , hit_number(0)
{
    this->setText(CALL_TREE_FUNCTION_COLUMN, function_name);
}

void MyItem::update_percentage()
{
    setData(CALL_TREE_SAMLPES_COLUMN, 0, QVariant((int)hit_number));
    setTextAlignment(CALL_TREE_SAMLPES_COLUMN, Qt::AlignRight);
    setData(CALL_TREE_PERCENTAGE_COLUMN, 0, QVariant(percentage()));
    setTextAlignment(CALL_TREE_PERCENTAGE_COLUMN, Qt::AlignRight);
    if (percentage() < 0.5)
    {
        QBrush brush(QColor(Qt::gray));
        setForeground(CALL_TREE_FUNCTION_COLUMN, brush);
        setForeground(CALL_TREE_SAMLPES_COLUMN, brush);
        setForeground(CALL_TREE_PERCENTAGE_COLUMN, brush);
    }
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->update_percentage();
    }
}

void MyItem::expand_all_greater_than(QTreeWidget *widget, size_t limit)
{
    if (hit_number <= limit)
        return;
    widget->expandItem(this);
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->expand_all_greater_than(widget, limit);
    }
}

void MyItem::expand_all(QTreeWidget *widget)
{
    widget->expandItem(this);
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->expand_all(widget);
    }
}

size_t MyItem::hits()
{
    return hit_number;
}

double MyItem::percentage()
{
    size_t total = ctx->total_hits();
    if (total == 0)
        return 100.;
    return 100. * hit_number / total;
}

void MyItem::touch()
{
    ++hit_number;
    //setText(1, QString::number(hit_number));
}

MyItem *MyItem::push(const char* function_name)
{
    touch();

    auto i = children.find(function_name);
    if (i == children.end())
    {
        MyItem* child = new MyItem(ctx, QString(function_name));
        i = children.insert(i, std::make_pair(function_name, child));
        this->insertChild(childCount(), child);
        return child;
    }
    else
    {
        return i->second;
    }
}
