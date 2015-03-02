#include <cassert>
#include <cstring>
#include <iostream>

#include "demangle.h"

int main(int argc, char* argv[])
{
    assert(argc != 0);
    for (int i = 1; i != argc; ++i)
    {
        try
        {
            demangling_result res = demangle(string_ref(argv[i], argv[i] + strlen(argv[i])));
            std::cout << res.demangled << "\n";
        }
        catch (demangling_error const&)
        {
            std::cout << "\n";
            std::cerr << "demangling error: " << argv[i] << "\n";
        }
    }
}
