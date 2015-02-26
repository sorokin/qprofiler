#include "string_ref.h"

#include <algorithm>

string_ref::string_ref()
    : first(nullptr)
    , last(nullptr)
{}

string_ref::string_ref(const char *begin, const char *end)
    : first(begin)
    , last(end)
{}

string_ref::string_ref(const std::string &str)
    : first(str.c_str())
    , last(first + str.size())
{}

const char *string_ref::begin() const
{
    return first;
}

const char *string_ref::end() const
{
    return last;
}

size_t string_ref::size() const
{
    return last - first;
}

size_t string_ref_value_hash::operator()(string_ref const& ref) const
{
    size_t res = 11;

    for (char a : ref)
    {
        res *= 17;
        res += (int)(unsigned char)a;
    }

    return res;
}


bool string_ref_value_compare::operator()(const string_ref &a, const string_ref &b) const
{
    if (a.size() != b.size())
        return false;

    return std::equal(a.begin(), a.end(), b.begin());
}
