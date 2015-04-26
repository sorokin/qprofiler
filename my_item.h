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

struct my_item : QTreeWidgetItem
{
    my_item(MyContext* ctx, profile::frame_index_type findex, QString const& function_name, QString const& dso_name);

    void update_percentage();
    void expand_all_greater_than(QTreeWidget* widget, size_t limit);
    void expand_all(QTreeWidget* widget);

    profile::frame_index_type frame_index() const;
    size_t self_hits() const;
    size_t hits() const;

    double self_percentage() const;
    double percentage() const;

    void touch();

    my_item* push(profile* p, profile::frame_index_type findex);

private:
    MyContext* ctx;
    profile::frame_index_type findex;
    size_t self_hit_number;
    size_t hit_number;

    std::unordered_map<profile::frame_index_type, my_item*, profile::frame_index_type::hash> children;
};

#endif // MY_ITEM_H
