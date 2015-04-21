#ifndef TUPLE_EQUALS_H
#define TUPLE_EQUALS_H

#include <tuple>
#include <functional>

#include "type_list.h"


template <typename Tuple, typename Comparers, size_t i, size_t N>
struct tuple_equals_impl
{
    static_assert(std::tuple_size<Tuple>::value == N, "value mismatch");
    static_assert(std::tuple_size<Comparers>::value == N, "value mismatch");
    static_assert(i < N, "invalid argument");

    static bool do_compare(Tuple const& a, Tuple const& b, Comparers const& c)
    {
        return std::get<i>(c)(std::get<i>(a), std::get<i>(b))
            && tuple_equals_impl<Tuple, Comparers, i + 1, N>::do_compare(a, b, c);
    }
};

template <typename Tuple, typename Comparers, size_t N>
struct tuple_equals_impl<Tuple, Comparers, N, N>
{
    static_assert(std::tuple_size<Tuple>::value == N, "value mismatch");
    static_assert(std::tuple_size<Comparers>::value == N, "value mismatch");

    static bool do_compare(Tuple const& a, Tuple const& b, Comparers const& c)
    {
        return true;
    }
};

template <typename Types, typename Comparers>
struct tuple_equals;

template <typename... Types, typename... Comparers>
struct tuple_equals<type_list<Types...>, type_list<Comparers...>>
{
    static_assert(sizeof...(Types) == sizeof...(Comparers), "size mismatch");

    typedef std::tuple<Types...> argument_type;
    typedef std::tuple<Comparers...> comparers_type;

    tuple_equals()
    {}

    tuple_equals(Comparers... comparers)
        : comparers(std::move(comparers)...)
    {}

    bool operator()(argument_type const& a, argument_type const& b) const
    {
        return tuple_equals_impl<argument_type,
                                 comparers_type,
                                 0,
                                 std::tuple_size<argument_type>::value>::do_compare(a, b, comparers);
    }

private:
    comparers_type comparers;
};

#endif
