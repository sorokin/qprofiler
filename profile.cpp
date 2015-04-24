#include "profile.h"

#include <fstream>
#include "my_item.h"

namespace
{
    template <typename ForwardIterator>
    void insert_trace(profile* p, MyItem* c, ForwardIterator first, ForwardIterator last)
    {
        for (auto i = first; i != last; ++i)
            c = c->push(p, *i);
        c->touch();
    }
}

bool is_whitespace(char c)
{
    return c <= ' ';
}

bool is_not_whitespace(char c)
{
    return c > ' ';
}

bool starts_with(std::string const& a, std::string const& b)
{
    size_t size = b.size();
    if (a.size() < size)
        return false;

    return std::equal(a.begin(), a.begin() + size, b.begin());
}

template <size_t N>
bool starts_with(std::string const& a, char const (&b)[N])
{
    if (a.size() < N)
        return false;

    return std::equal(a.begin(), a.begin() + N, static_cast<char const*>(b));
}

template <size_t N>
bool starts_with(string_ref const& a, char const (&b)[N])
{
    if (a.size() < N)
        return false;

    return std::equal(a.begin(), a.begin() + N, static_cast<char const*>(b));
}

profile::profile()
{}

void profile::build_tree(MyItem* root)
{
    for (auto i = samples.begin(); i != samples.end(); ++i)
    {
        auto& funcs = i->frames;
        auto j = funcs.crbegin();
        if (j != funcs.crend() && starts_with(frame_pool[*j].function_name, "[unknown]"))
            ++j;

        insert_trace(this, root, j, funcs.crend());
    }
}

void profile::build_reverse_tree(MyItem* root)
{
    for (auto i = samples.begin(); i != samples.end(); ++i)
    {
        auto& funcs = i->frames;
        insert_trace(this, root, funcs.cbegin(), funcs.cend());
    }
}

void profile::build_tree_function(MyItem* root, frame_index_type index)
{
    for (auto i = samples.begin(); i != samples.end(); ++i)
    {
        auto& funcs = i->frames;
        auto j = std::find(funcs.crbegin(), funcs.crend(), index);
        insert_trace(this, root, j, funcs.crend());
    }
}

const profile::frame &profile::get_frame(frame_index_type index)
{
    return frame_pool[index];
}

profile::frame::frame(string_ref function_name, string_ref dso_name)
    : function_name(function_name)
    , dso_name(dso_name)
{}

profile::backtrace::backtrace(std::vector<frame_index_type> frames)
    : frames(std::move(frames))
{}

constexpr profile::frame_index_type profile::invalid_frame_index;
