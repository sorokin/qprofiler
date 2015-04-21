#ifndef STRING_REF_H
#define STRING_REF_H

#include <cstdlib>
#include <string>

struct string_ref
{
    string_ref();
    string_ref(char const* begin, char const* end);
    explicit string_ref(std::string const& str);

    char const* begin() const;
    char const* end() const;
    size_t size() const;

private:
    char const* first;
    char const* last;
};

struct string_ref_value_hash
{
    typedef string_ref argument_type;

    size_t operator()(string_ref const& a) const;
};

struct string_ref_value_compare
{
    bool operator()(string_ref const& a, string_ref const& b) const;
};

#endif
