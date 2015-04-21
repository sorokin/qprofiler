#include "profile.h"

#include <fstream>
#include "my_item.h"

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
        MyItem* c = root;
        for (auto i = funcs.crbegin(); i != funcs.crend(); ++i)
        {
            if (i == funcs.rbegin() && starts_with(frame_pool[*i].function_name, "[unknown]"))
                continue;

            c = c->push(this, *i);
        }
        c->touch();
    }
}

void profile::build_reverse_tree(MyItem* root)
{
    for (auto i = samples.begin(); i != samples.end(); ++i)
    {
        auto& funcs = i->frames;
        MyItem* c = root;
        for (auto i = funcs.cbegin(); i != funcs.cend(); ++i)
        {
            c = c->push(this, *i);
        }
        c->touch();
    }
}

const profile::frame &profile::get_frame(profile::frame_index_type index)
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

