#ifndef MY_ITEM_H
#define MY_ITEM_H

#include <QTreeWidgetItem>
#include <QString>
#include <string>
#include <unordered_map>
#include <utility>

#include "string_ref.h"
#include "profile.h"

struct MyContext;

struct key
{
    key(char const* first, char const* second);

    char const* first;
    char const* second;
};

struct key_hash_value;
struct key_compare;

struct key_hash_value
{
    size_t operator()(key const& a) const
    {
        return 97 + 17 * std::hash<char const*>()(a.first) + 31 * std::hash<char const*>()(a.second);
    }
};

struct key_compare
{
    bool operator()(key const& a, key const& b) const
    {
        return a.first == b.first
            && a.second == b.second;
    }
};

struct MyItem : QTreeWidgetItem
{
    MyItem(MyContext* ctx, QString const& function_name, QString const& dso_name);

    void update_percentage();
    void expand_all_greater_than(QTreeWidget* widget, size_t limit);
    void expand_all(QTreeWidget* widget);

    size_t hits();

    double percentage();

    void touch();

    MyItem* push(profile* p, profile::frame_index_type findex);

private:
    MyContext* ctx;
    size_t hit_number;

    std::unordered_map<profile::frame_index_type, MyItem*, profile::frame_index_type::hash> children;
};

#endif // MY_ITEM_H
