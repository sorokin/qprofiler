#include "demangle.h"

#include <algorithm>

namespace
{
    enum cv_qualifiers
    {
        QUAL_NONE     = 0,
        QUAL_CONST    = 1,
        QUAL_VOLATILE = 2,
    };

    size_t read_number(char const*& pos, char const* end)
    {
        char const* start = pos;
        size_t res = 0;

        for (;;)
        {
            if (pos == end)
                break;

            char c = *pos;
            if (c < '0' || c > '9')
                break;

            res *= 10; // TODO: check for overflow
            res += (c - '0');
            ++pos;
        }

        if (start == pos)
            throw demangling_error();

        return res;
    }

    void read_string(char const*& pos, char const* end, size_t n, std::string& outbuf, std::pair<size_t, size_t>* name_range)
    {
        if ((end - pos) < n)
            throw demangling_error();

        if (name_range)
            *name_range = std::make_pair(outbuf.size(), outbuf.size() + n);

        outbuf.append(pos, pos + n);
        pos += n;
    }

    void read_cv_qualifiers(char const*& pos, char const* end, cv_qualifiers* cv_quals)
    {
        if (pos == end)
            return;

        if (*pos == 'V')
        {
            if (!cv_quals)
                throw demangling_error();

            *cv_quals = cv_qualifiers(*cv_quals | QUAL_VOLATILE);
            ++pos;
        }

        if (pos == end)
            return;

        if (*pos == 'K')
        {
            if (!cv_quals)
                throw demangling_error();

            *cv_quals = cv_qualifiers(*cv_quals | QUAL_CONST);
            ++pos;
        }
    }

    void demangle_name(char const*& pos, char const* end, std::string& outbuf, std::pair<size_t ,size_t>* name_range, cv_qualifiers* cv_quals)
    {
        if (pos == end)
            throw demangling_error();

        if (*pos == 'N')
        {
            ++pos;

            read_cv_qualifiers(pos, end, cv_quals);

            for (;;)
            {
                std::pair<size_t, size_t> nr;

                size_t n = read_number(pos, end);
                read_string(pos, end, n, outbuf, &nr);

                if (pos == end)
                    throw demangling_error();

                if (*pos == 'E')
                {
                    ++pos;
                    if (name_range)
                        *name_range = nr;
                    break;
                }
                else
                    outbuf += "::";
            }
        }
        else
        {
            size_t n = read_number(pos, end);
            read_string(pos, end, n, outbuf, name_range);
        }
    }

    void demangle_function_parameters(char const*& pos, char const* end, std::string& outbuf);

    enum rhs_type
    {
        none,
        function,
        array,
        ptr_to_function,
        function_returning_ptr_to_function,
    };

    template <typename F, typename FF>
    rhs_type demangle_type_rec(char const*& pos, char const* end, F out, FF prepend_suffix)
    {
        if (pos == end)
            throw demangling_error();

        char c = *pos;

        switch (c)
        {
        case 'a':
            out("signed char");
            ++pos;
            return none;
        case 'b':
            out("bool");
            ++pos;
            return none;
        case 'c':
            out("char");
            ++pos;
            return none;
        case 'd':
            out("double");
            ++pos;
            return none;
        case 'e':
            out("long double");
            ++pos;
            return none;
        case 'f':
            out("float");
            ++pos;
            return none;
        case 'g':
            out("__float128");
            ++pos;
            return none;
        case 'h':
            out("unsigned char");
            ++pos;
            return none;
        case 'i':
            out("int");
            ++pos;
            return none;
        case 'j':
            out("unsigned int");
            ++pos;
            return none;
        case 'k':
            throw demangling_error();
        case 'l':
            out("long");
            ++pos;
            return none;
        case 'm':
            out("unsigned long");
            ++pos;
            return none;
        case 'n':
            out("__int128");
            ++pos;
            return none;
        case 'o':
            out("unsigned __int128");
            ++pos;
            return none;
        case 'p':
        case 'q':
        case 'r':
            throw demangling_error();
        case 's':
            out("short");
            ++pos;
            return none;
        case 't':
            out("unsigned short");
            ++pos;
            return none;
        case 'u':
            throw demangling_error();
        case 'v':
            out("void");
            ++pos;
            return none;
        case 'w':
            out("wchar_t");
            ++pos;
            return none;
        case 'x':
            out("long long");
            ++pos;
            return none;
        case 'y':
            out("unsigned long long");
            ++pos;
            return none;
        case 'z':
            out("...");
            ++pos;
            return none;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'N':
            {
                std::string buf; // TODO
                demangle_name(pos, end, buf, nullptr, nullptr);
                out(buf.c_str());
                return none;
            }

        case 'P':
        case 'R':
        case 'O':
            {
                ++pos;

                cv_qualifiers cv_quals = QUAL_NONE;
                read_cv_qualifiers(pos, end, &cv_quals);

                rhs_type rhs = demangle_type_rec(pos, end, out, prepend_suffix);
                if (rhs == function)
                {
                    out(" (");
                    prepend_suffix(")");
                }
                else if (rhs == array)
                {
                    out(" (");
                    prepend_suffix(") ");
                }
                else if (rhs == function_returning_ptr_to_function)
                {
                    out("(");
                    prepend_suffix(")");
                }

                if (cv_quals & QUAL_CONST)
                    out(" const");
                if (cv_quals & QUAL_VOLATILE)
                    out(" volatile");

                out(c == 'P' ? "*" :
                    c == 'R' ? "&" :
                    "&&");
                return rhs == function ? ptr_to_function : none;
            }
        case 'F':
            {
                ++pos;
                rhs_type rhs = demangle_type_rec(pos, end, out, prepend_suffix);
                std::string params;
                demangle_function_parameters(pos, end, params);
                prepend_suffix(params);
                if (pos == end)
                    throw demangling_error();
                if (*pos != 'E')
                    throw demangling_error();
                ++pos;
                return rhs == ptr_to_function ? function_returning_ptr_to_function : function;
            }
        case 'A':
            {
                ++pos;
                char const* start = pos;
                char const* size_end;
                for (;;)
                {
                    if (pos == end)
                        throw demangling_error();

                    if (*pos != '_')
                        ++pos;
                    else
                    {
                        size_end = pos;
                        ++pos;
                        break;
                    }
                }
                demangle_type_rec(pos, end, out, prepend_suffix);
                prepend_suffix("]");
                prepend_suffix(std::string(start, size_end));
                prepend_suffix("[");
                return array;
            }
        default:
            throw demangling_error();
        }
    }

    template <typename Out>
    void demangle_type(char const*& pos, char const* end, Out out)
    {
        std::string suffix;
        demangle_type_rec(pos, end, out, [&](std::string text) {
            suffix = std::move(text) + suffix;
        });

        out(suffix.c_str());
    }

    void demangle_function_parameters(char const*& pos, char const* end, std::string& outbuf)
    {
        if (pos == end)
            throw demangling_error();

        if (*pos == 'v')
        {
            outbuf += "()";
            ++pos;
            return;
        }

        outbuf += '(';

        bool first = true;
        for (;;)
        {
            if (pos == end)
                break;
            if (*pos == 'E')
                break;

            if (!first)
                outbuf += ", ";
            else
                first = false;

            demangle_type(pos, end, [&] (char const* type) {
                outbuf += type;
            });
        }

        outbuf += ')';
    }
}

demangling_result::demangling_result()
{}

demangling_result::demangling_result(std::string demangled, std::pair<size_t, size_t> function_name_range)
    : demangled(demangled)
    , function_name_range(function_name_range)
{}

demangling_result demangle(string_ref const& str)
{
    demangling_result res;

    char const* pos = str.begin();
    char const* const end = str.end();

    if (end - pos < 2 || pos[0] != '_' || pos[1] != 'Z')
        throw demangling_error();
    pos += 2;

    cv_qualifiers cv_quals = QUAL_NONE;
    demangle_name(pos, end, res.demangled, &res.function_name_range, &cv_quals);
    demangle_function_parameters(pos, end, res.demangled);

    if (cv_quals & QUAL_CONST)
        res.demangled += " const";

    if (cv_quals & QUAL_VOLATILE)
        res.demangled += " volatile";

    if (pos != end)
        throw demangling_error();

    return res;
}
