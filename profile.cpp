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
        for (std::vector<string_ref>::const_reverse_iterator i = funcs.rbegin(); i != funcs.rend(); ++i)
        {
            if (i == funcs.rbegin() && starts_with(*i, "[unknown]"))
                continue;

            c = c->push(i->begin());
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
        for (std::vector<string_ref>::const_iterator i = funcs.begin(); i != funcs.end(); ++i)
        {
            c = c->push(i->begin());
        }
        c->touch();
    }
}

profile::backtrace::backtrace(std::vector<string_ref> frames)
    : frames(frames)
{}
