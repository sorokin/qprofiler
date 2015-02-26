#ifndef STRING_POOL_H
#define STRING_POOL_H

#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>

#include "string_ref.h"

struct string_pool
{
    string_pool(size_t initial_capacity = 32);
    string_ref put(std::string const& str);
    string_ref put(std::string&& str);
    string_ref put(string_ref const& str_ref);

private:
    std::vector<char> data;
    std::unordered_map<string_ref, std::string, string_ref_value_hash, string_ref_value_compare> map;
};

#endif // STRING_POOL_H
