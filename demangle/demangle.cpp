#include "demangle.h"

#include <algorithm>
#include <cassert>

namespace
{
    typedef size_t seqid;
    static seqid const null_seqid = std::numeric_limits<seqid>::max();
    static seqid const type_seqid = std::numeric_limits<seqid>::max() - 1;

    struct subst
    {
        subst(seqid qualifier, string_ref name)
            : qualifier(qualifier)
            , name(name)
        {}

        seqid qualifier;
        string_ref name;
    };

    struct demangle_context
    {
        seqid push(seqid qualifier, string_ref name)
        {
            size_t r = substs.size();
            substs.push_back(subst(qualifier, name));
            return r;
        }

        std::vector<subst> substs;
    };

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

    void out_string(char const* start, char const* end, std::string& outbuf, std::pair<size_t, size_t>* name_range)
    {
        if (name_range)
            *name_range = std::make_pair(outbuf.size(), outbuf.size() + (end - start));

        outbuf.append(start, end);
    }

    void read_string(char const*& pos, char const* end, size_t n, std::string& outbuf, std::pair<size_t, size_t>* name_range)
    {
        if ((end - pos) < n)
            throw demangling_error();

        out_string(pos, pos + n, outbuf, name_range);
        pos += n;
    }

    cv_qualifiers read_cv_qualifiers(char const*& pos, char const* end)
    {
        cv_qualifiers quals = QUAL_NONE;

        if (pos == end)
            return quals;

        if (*pos == 'V')
        {
            quals = cv_qualifiers(quals | QUAL_VOLATILE);
            ++pos;
        }

        if (pos == end)
            return quals;

        if (*pos == 'K')
        {
            quals = cv_qualifiers(quals | QUAL_CONST);
            ++pos;
        }

        return quals;
    }

    size_t base36decode(char value)
    {
        if (value >= '0' && value <= '9')
            return value - '0';
        if (value >= 'A' && value <= 'Z')
            return value - 'A' + 10;
        throw demangling_error();
    }

    size_t read_seqid_number(char const*& pos, char const* end)
    {
        if (*pos == '_')
        {
            ++pos;
            return 0;
        }

        size_t acc = 0;
        for (;;)
        {
            if (pos == end)
                throw demangling_error();

            if (*pos == '_')
            {
                ++pos;
                break;
            }

            acc *= 36; // TODO check overflow
            acc += base36decode(*pos);
            ++pos;
        }

        return acc + 1;
    }

    void print_seqid(demangle_context& ctx, size_t i, std::string& outbuf);

    seqid demangle_seqid(demangle_context& ctx, char const*& pos, char const* end, std::string& outbuf)
    {
        assert(pos != end && *pos == 'S');
        ++pos;

        seqid id = read_seqid_number(pos, end);
        if (id >= ctx.substs.size())
            throw demangling_error();
        print_seqid(ctx, id, outbuf);
        return id;
    }

    void demangle_type_name(demangle_context& ctx, char const*& pos, char const* end, std::string& outbuf)
    {
        if (pos == end)
            throw demangling_error();

        if (*pos == 'N')
        {
            ++pos;

            if (pos == end)
                throw demangling_error();

            seqid parent = null_seqid;

            if (*pos == 'S')
            {
                parent = demangle_seqid(ctx, pos, end, outbuf);
                outbuf += "::";
            }

            for (;;)
            {
                std::pair<size_t, size_t> nr;

                size_t n = read_number(pos, end);
                char const* name_start = pos;
                read_string(pos, end, n, outbuf, &nr);
                char const* name_end = pos;
                parent = ctx.push(parent, string_ref(name_start, name_end));

                if (*pos == 'E')
                {
                    ++pos;
                    break;
                }
                else
                    outbuf += "::";
            }
        }
        else if (*pos == 'S')
        {
            demangle_seqid(ctx, pos, end, outbuf);
        }
        else
        {
            size_t n = read_number(pos, end);
            char const* name_start = pos;
            read_string(pos, end, n, outbuf, nullptr);
            char const* name_end = pos;
            ctx.push(null_seqid, string_ref(name_start, name_end));
        }
    }

    void demangle_function_name(demangle_context& ctx, char const*& pos, char const* end, std::string& outbuf, std::pair<size_t ,size_t>* name_range, cv_qualifiers* cv_quals)
    {
        if (pos == end)
            throw demangling_error();

        if (*pos == 'N')
        {
            ++pos;

            seqid parent = null_seqid;
            *cv_quals = read_cv_qualifiers(pos, end);

            for (;;)
            {
                std::pair<size_t, size_t> nr;

                size_t n = read_number(pos, end);
                char const* const name_start = pos;
                read_string(pos, end, n, outbuf, &nr);
                char const* const name_end = pos;

                if (pos == end)
                    throw demangling_error();

                if (*pos == 'C')
                {
                    ++pos;
                    parent = ctx.push(parent, string_ref(name_start, name_end));
                    outbuf += "::";
                    out_string(name_start, name_end, outbuf, name_range);

                    if (pos == end)
                        throw demangling_error();
                    if (*pos != '1' && *pos != '2' && *pos != '3')
                        throw demangling_error();
                    ++pos;
                    if (pos == end)
                        throw demangling_error();
                    if (*pos != 'E')
                        throw demangling_error();
                    ++pos;
                    break;
                }
                else if (*pos == 'D')
                {
                    ++pos;
                    parent = ctx.push(parent, string_ref(name_start, name_end));
                    outbuf += "::~";
                    out_string(name_start, name_end, outbuf, name_range);

                    if (pos == end)
                        throw demangling_error();
                    if (*pos != '0' && *pos != '1' && *pos != '2')
                        throw demangling_error();
                    ++pos;
                    if (pos == end)
                        throw demangling_error();
                    if (*pos != 'E')
                        throw demangling_error();
                    ++pos;
                    break;
                }
                else if (*pos == 'E')
                {
                    ++pos;
                    if (name_range)
                        *name_range = nr;
                    break;
                }
                else
                {
                    parent = ctx.push(parent, string_ref(name_start, name_end));

                    outbuf += "::";
                }
            }
        }
        else
        {
            size_t n = read_number(pos, end);
            read_string(pos, end, n, outbuf, name_range);
        }
    }

    void demangle_function_parameters(demangle_context& ctx, char const*& pos, char const* end, std::string& outbuf);

    enum rhs_type
    {
        none,
        function,
        array,
        ptr_to_function,
        function_returning_ptr_to_function,
    };

    template <typename F, typename FF>
    rhs_type demangle_type_rec(demangle_context& ctx, char const*& pos, char const* end, F out, FF prepend_suffix)
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
        case 'S':
            {
                std::string buf; // TODO
                demangle_type_name(ctx, pos, end, buf);
                out(buf.c_str());
                return none;
            }
        case 'K':
        case 'V':
            {
                char const* const type_start = pos;
                cv_qualifiers cv_quals = read_cv_qualifiers(pos, end);
                rhs_type rhs = demangle_type_rec(ctx, pos, end, out, prepend_suffix);
                char const* const type_end = pos;
                ctx.push(type_seqid, string_ref(type_start, type_end));
                if (cv_quals & QUAL_CONST)
                    out(" const");
                if (cv_quals & QUAL_VOLATILE)
                    out(" volatile");
                return rhs;
            }
        case 'P':
        case 'R':
        case 'O':
            {
                char const* const type_start = pos;
                ++pos;

                rhs_type rhs = demangle_type_rec(ctx, pos, end, out, prepend_suffix);
                char const* const type_end = pos;
                ctx.push(type_seqid, string_ref(type_start, type_end));
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

                out(c == 'P' ? "*" :
                    c == 'R' ? "&" :
                    "&&");
                return rhs == function ? ptr_to_function : none;
            }
        case 'F':
            {
                char const* type_start = pos;
                ++pos;
                rhs_type rhs = demangle_type_rec(ctx, pos, end, out, prepend_suffix);
                std::string params;
                demangle_function_parameters(ctx, pos, end, params);
                char const* type_end = pos;
                ctx.push(type_seqid, string_ref(type_start, type_end));
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
                char const* type_start = pos;
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
                demangle_type_rec(ctx, pos, end, out, prepend_suffix);
                char const* type_end = pos;
                ctx.push(type_seqid, string_ref(type_start, type_end));
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
    void demangle_type(demangle_context& ctx, char const*& pos, char const* end, Out out)
    {
        std::string suffix;
        demangle_type_rec(ctx, pos, end, out, [&](std::string text) {
            suffix = std::move(text) + suffix;
        });

        out(suffix.c_str());
    }

    void print_seqid(demangle_context& ctx, size_t i, std::string& outbuf)
    {
        subst const& sub = ctx.substs[i];
        if (sub.qualifier == type_seqid)
        {
            char const* pos = sub.name.begin();
            demangle_type(ctx, pos, sub.name.end(), [&](char const* msg) {
                outbuf += msg;
            });
            return;
        }
        if (sub.qualifier != null_seqid)
        {
            print_seqid(ctx, sub.qualifier, outbuf);
            outbuf += "::";
        }
        outbuf.insert(outbuf.end(), sub.name.begin(), sub.name.end());
    }

    void demangle_function_parameters(demangle_context& ctx, char const*& pos, char const* end, std::string& outbuf)
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

            demangle_type(ctx, pos, end, [&] (char const* type) {
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
    demangle_context ctx;

    char const* pos = str.begin();
    char const* const end = str.end();

    if (end - pos < 2 || pos[0] != '_' || pos[1] != 'Z')
        throw demangling_error();
    pos += 2;

    cv_qualifiers cv_quals = QUAL_NONE;
    demangle_function_name(ctx, pos, end, res.demangled, &res.function_name_range, &cv_quals);
    demangle_function_parameters(ctx, pos, end, res.demangled);

    if (cv_quals & QUAL_CONST)
        res.demangled += " const";

    if (cv_quals & QUAL_VOLATILE)
        res.demangled += " volatile";

    if (pos != end)
        throw demangling_error();

    return res;
}
