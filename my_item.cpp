#include "my_item.h"
#include "call_tree_column.h"
#include "my_context.h"
#include "demangle/demangle.h"


my_item::my_item(MyContext *ctx, profile::frame_index_type findex, QString const& function_name, QString const& dso_name)
    : ctx(ctx)
    , findex(findex)
    , self_hit_number(0)
    , hit_number(0)
{
    setToolTip(0, dso_name);
    this->setText(CALL_TREE_FUNCTION_COLUMN, function_name);
}

void my_item::update_percentage()
{
    setData(CALL_TREE_SAMPLES_COLUMN, 0, QVariant((int)hit_number));
    setTextAlignment(CALL_TREE_SAMPLES_COLUMN, Qt::AlignRight);
    setData(CALL_TREE_PERCENTAGE_COLUMN, 0, QVariant(percentage()));
    setTextAlignment(CALL_TREE_PERCENTAGE_COLUMN, Qt::AlignRight);
    setData(CALL_TREE_SELF_SAMPLES_COLUMN, 0, QVariant((int)self_hit_number));
    setTextAlignment(CALL_TREE_SELF_SAMPLES_COLUMN, Qt::AlignRight);
    setData(CALL_TREE_SELF_PERCENTAGE_COLUMN, 0, QVariant(self_percentage()));
    setTextAlignment(CALL_TREE_SELF_PERCENTAGE_COLUMN, Qt::AlignRight);
    if (percentage() < 0.5)
    {
        QBrush brush(QColor(Qt::gray));
        setForeground(CALL_TREE_FUNCTION_COLUMN, brush);
        setForeground(CALL_TREE_SAMPLES_COLUMN, brush);
        setForeground(CALL_TREE_PERCENTAGE_COLUMN, brush);
    }
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->update_percentage();
    }
}

void my_item::expand_all_greater_than(QTreeWidget *widget, size_t limit)
{
    if (hit_number <= limit)
        return;
    widget->expandItem(this);
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->expand_all_greater_than(widget, limit);
    }
}

void my_item::expand_all(QTreeWidget *widget)
{
    widget->expandItem(this);
    for (auto i = children.cbegin(); i != children.cend(); ++i)
    {
        i->second->expand_all(widget);
    }
}

profile::frame_index_type my_item::frame_index() const
{
    return findex;
}

size_t my_item::self_hits() const
{
    return self_hit_number;
}

size_t my_item::hits() const
{
    return hit_number;
}

double my_item::self_percentage() const
{
    size_t total = ctx->total_hits();
    if (total == 0)
        return 100.;
    return 100. * self_hit_number / total;
}

double my_item::percentage() const
{
    size_t total = ctx->total_hits();
    if (total == 0)
        return 100.;
    return 100. * hit_number / total;
}

void my_item::touch()
{
    ++self_hit_number;
    ++hit_number;
    //setText(1, QString::number(hit_number));
}

my_item *my_item::push(profile* p, profile::frame_index_type findex)
{
    ++hit_number;

    auto i = children.find(findex);
    if (i == children.end())
    {
        profile::frame const& frame = p->get_frame(findex);
        std::string demangled;
        try
        {
            demangling_result res = demangle(frame.function_name);
            demangled = std::move(res.demangled);
        }
        catch (demangling_error const&)
        {
            demangled = frame.function_name.begin();
        }

        my_item* child = new my_item(ctx, findex, QString::fromStdString(demangled), QString::fromLocal8Bit(frame.dso_name.begin()));
        i = children.insert(i, std::make_pair(findex, child));
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
