#include "my_item.h"
#include "call_tree_column.h"
#include "my_context.h"
#include "demangle/demangle.h"


MyItem::MyItem(MyContext *ctx, QString const& function_name, QString const& dso_name)
    : ctx(ctx)
    , hit_number(0)
{
    setToolTip(0, dso_name);
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

MyItem *MyItem::push(const char* function_name, char const* dso_name)
{
    touch();

    auto i = children.find(key_t(function_name, dso_name));
    if (i == children.end())
    {
        std::string demangled;
        try
        {
            demangling_result res = demangle(string_ref(function_name, function_name + strlen(function_name)));
            demangled = std::move(res.demangled);
        }
        catch (demangling_error const&)
        {
            demangled = function_name;
        }

        MyItem* child = new MyItem(ctx, QString::fromStdString(demangled), QString::fromLocal8Bit(dso_name));
        i = children.insert(i, std::make_pair(key_t(function_name, dso_name), child));
        this->insertChild(childCount(), child);
        return child;
    }
    else
    {
        return i->second;
    }
}


key::key(const char *first, const char *second)
    : first(first)
    , second(second)
{}
