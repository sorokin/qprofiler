#include "demangle.h"
#include <iostream>

size_t number_of_test_passed = 0;
size_t number_of_test_failed = 0;

void test_demangling(std::string const& input, std::string const& expected)
{
    try
    {
        demangling_result res = demangle(string_ref(input));
        if (res.demangled != expected)
        {
            std::cerr << "demangling failed\n"
                      << "  input:    " << input
                      << "  output:   " << res.demangled
                      << "  expected: " << expected
                      << "\n";
            ++number_of_test_failed;
        }
        else
            ++number_of_test_passed;
    }
    catch (demangling_error const&)
    {
        std::cerr << "demangling error\n"
                  << "  input:    " << input
                  << "\n";
        ++number_of_test_failed;
    }
}

int main()
{
    test_demangling("_Z1fv",                                "f()");
    test_demangling("_Z2fov",                               "fo()");
    test_demangling("_Z3foov",                              "foo()");
    test_demangling("_Z26abcdefghijklmnopqrstuvwxyzv",      "abcdefghijklmnopqrstuvwxyz()");

    test_demangling("_Z3fooi",                              "foo(int)");
    test_demangling("_Z3fooii",                             "foo(int, int)");
    test_demangling("_Z3fooiii",                            "foo(int, int, int)");

    test_demangling("_Z3bara",                              "bar(signed char)");
    test_demangling("_Z3barb",                              "bar(bool)");
    test_demangling("_Z3barc",                              "bar(char)");
    test_demangling("_Z3bard",                              "bar(double)");
    test_demangling("_Z3bare",                              "bar(long double)");
    test_demangling("_Z3barf",                              "bar(float)");
    test_demangling("_Z3barg",                              "bar(__float128)");
    test_demangling("_Z3barh",                              "bar(unsigned char)");
    test_demangling("_Z3bari",                              "bar(int)");
    test_demangling("_Z3barj",                              "bar(unsigned int)");
    test_demangling("_Z3barl",                              "bar(long)");
    test_demangling("_Z3barm",                              "bar(unsigned long)");
    test_demangling("_Z3barn",                              "bar(__int128)");
    test_demangling("_Z3baro",                              "bar(unsigned __int128)");
    test_demangling("_Z3bars",                              "bar(short)");
    test_demangling("_Z3bart",                              "bar(unsigned short)");
    test_demangling("_Z3barw",                              "bar(wchar_t)");
    test_demangling("_Z3barx",                              "bar(long long)");
    test_demangling("_Z3bary",                              "bar(unsigned long long)");
    test_demangling("_Z3barz",                              "bar(...)");
    test_demangling("_Z3bar6mytype",                        "bar(mytype)");
    test_demangling("_Z3barN6mytype6nestedE",               "bar(mytype::nested)");

    test_demangling("_Z3quxPi",                             "qux(int*)");
    test_demangling("_Z3quxPKi",                            "qux(int const*)");
    test_demangling("_Z3quxPVi",                            "qux(int volatile*)");
    test_demangling("_Z3quxPVKi",                           "qux(int const volatile*)");
    test_demangling("_Z3quxRi",                             "qux(int&)");
    test_demangling("_Z3quxOi",                             "qux(int&&)");
    test_demangling("_Z3quxPPPi",                           "qux(int***)");
    test_demangling("_Z3quxRA10_i",                         "qux(int (&) [10])");
    test_demangling("_Z3quxRA10_PPi",                       "qux(int** (&) [10])");
    test_demangling("_Z3quxRA10_A20_i",                     "qux(int (&) [10][20])");
    test_demangling("_Z3quxPFvvE",                          "qux(void (*)())");
    test_demangling("_Z3quxPFviiiEPi",                      "qux(void (*)(int, int, int), int*)");
    test_demangling("_Z3quxPFPFvvEvE",                      "qux(void (*(*)())())");
    test_demangling("_Z3quxPFPvvE",                         "qux(void* (*)())");
    test_demangling("_Z3quxPFRA10_ivE",                     "qux(int (& (*)()) [10])");

    test_demangling("_ZN2ns3bazEv",                         "ns::baz()");
    test_demangling("_ZN3ns13ns23bazEv",                    "ns1::ns2::baz()");
    test_demangling("_ZN6mytype3barEv",                     "mytype::bar()");

    test_demangling("_ZNK6mytype5cargoEv",                  "mytype::cargo() const");
    test_demangling("_ZNV6mytype5vargoEv",                  "mytype::vargo() volatile");
    test_demangling("_ZNVK6mytype6cvargoEv",                "mytype::cvargo() const volatile");

    test_demangling("_ZN6mytypeC1ERKi",                     "mytype::mytype(int const&)");
    test_demangling("_ZN6mytypeC2ERKi",                     "mytype::mytype(int const&)");
    test_demangling("_ZN6mytypeC3ERKi",                     "mytype::mytype(int const&)");

    test_demangling("_ZN6mytypeD0Ev",                       "mytype::~mytype()");
    test_demangling("_ZN6mytypeD1Ev",                       "mytype::~mytype()");
    test_demangling("_ZN6mytypeD2Ev",                       "mytype::~mytype()");

    std::cerr << number_of_test_passed << " tests passed, " << number_of_test_failed << " tests failed\n";
}
