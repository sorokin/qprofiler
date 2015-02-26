#ifndef MY_ITEM_H
#define MY_ITEM_H

#include <QTreeWidgetItem>
#include <QString>
#include <string>
#include <unordered_map>

#include "string_ref.h"

struct MyContext;

struct MyItem : QTreeWidgetItem
{
    MyItem(MyContext* ctx, QString const& function_name);

    void update_percentage();
    void expand_all_greater_than(QTreeWidget* widget, size_t limit);
    void expand_all(QTreeWidget* widget);

    size_t hits();

    double percentage();

    void touch();

    MyItem* push(const char* function_name);

private:
    MyContext* ctx;
    size_t hit_number;
    std::unordered_map<const char*, MyItem*> children;
};

#endif // MY_ITEM_H
