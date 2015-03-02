#ifndef DEMANGLE_H
#define DEMANGLE_H

#include "string_ref.h"

struct demangling_result
{
    demangling_result();
    demangling_result(std::string demangled, std::pair<size_t, size_t> function_name_range);

    std::string demangled;
    std::pair<size_t, size_t> function_name_range;
};

struct demangling_error
{};

demangling_result demangle(string_ref const&);

#endif // DEMANGLE_H
