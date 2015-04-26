#include "profile.h"

#include <fstream>
#include <utility>
#include <cassert>
#include "my_item.h"
#include "transformation.h"

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

template <typename ForwardIterator>
void profile::insert_range(my_item* c, ForwardIterator first, ForwardIterator last)
{
    for (auto i = first; i != last; ++i)
        c = c->push(this, *i);
}

template <typename ForwardIterator>
void profile::insert_trace(my_item* root, ForwardIterator first, ForwardIterator last, transformation const& tr)
{
    ForwardIterator i = first;
    for (auto j = tr.roots.cbegin(); j != tr.roots.cend(); ++j)
    {
        i = std::find_if(i, last, [&tr, j](frame_index_type index) {
           return j->find(index) != j->end();
        });

        if (i == last)
            return;
    }

    insert_range(root, i, last);
}

void profile::build_tree(my_item* root, transformation const& tr)
{
    switch (tr.direction)
    {
    case transformation::direction_type::forward:
        for (auto i = samples.begin(); i != samples.end(); ++i)
        {
            auto const& funcs = i->frames;
            auto j = funcs.crbegin();
            if (j != funcs.crend() && starts_with(frame_pool[*j].function_name, "[unknown]"))
                ++j;

            insert_trace(root, j, funcs.crend(), tr);
        }
        break;
    case transformation::direction_type::backward:
        for (auto i = samples.begin(); i != samples.end(); ++i)
        {
            auto const& funcs = i->frames;
            insert_trace(root, funcs.cbegin(), funcs.cend(), tr);
        }
        break;
    default:
        assert(false);
        break;
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
