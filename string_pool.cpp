#include "string_pool.h"

string_pool::string_pool(size_t initial_capacity)
{
    data.reserve(initial_capacity);
}

string_ref string_pool::put(std::string const& str)
{
    return put(string_ref(str));
}

string_ref string_pool::put(std::string&& str)
{
    string_ref str_ref(str);
    auto i = map.find(str_ref);
    if (i == map.end())
        i = map.insert(i, std::make_pair(str_ref, std::move(str)));
    else
        str.clear();

    return string_ref(i->second);
}

string_ref string_pool::put(string_ref const& str_ref)
{
    auto i = map.find(str_ref);
    if (i == map.end())
    {
        std::string tmp(str_ref.begin(), str_ref.end());
        string_ref tmpref(tmp);
        i = map.insert(i, std::make_pair(tmpref, std::move(tmp)));
    }

    return string_ref(i->second);
}
