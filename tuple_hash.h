#ifndef TUPLE_HASH_H
#define TUPLE_HASH_H

#include <functional>
#include <tuple>

#include "type_list.h"

inline size_t hash_combine(size_t left, size_t right)
{
    return left + 0x9e3779b9 + (right<<6) + (right>>2);
}

template <typename Tuple, typename Hashers, size_t i, size_t N>
struct tuple_hash_impl
{
    static_assert(std::tuple_size<Tuple>::value == N, "value mismatch");
    static_assert(std::tuple_size<Hashers>::value == N, "value mismatch");
    static_assert(i < N, "invalid argument");

    static size_t do_hash(Tuple const& tuple, Hashers const& hashers)
    {
        size_t my = std::get<i>(hashers)(std::get<i>(tuple));
        size_t other = tuple_hash_impl<Tuple, Hashers, i + 1, N>::do_hash(tuple, hashers);
        return hash_combine(my, other);
    }
};

template <typename Tuple, typename Hashers, size_t N>
struct tuple_hash_impl<Tuple, Hashers, N, N>
{
    static_assert(std::tuple_size<Tuple>::value == N, "value mismatch");
    static_assert(std::tuple_size<Hashers>::value == N, "value mismatch");

    static size_t do_hash(Tuple const&, Hashers const&)
    {
        return 97;
    }
};

template <typename Types, typename Hashers>
struct tuple_hash;

template <typename... Types, typename... Hashers>
struct tuple_hash<type_list<Types...>, type_list<Hashers...> >
{
    static_assert(sizeof...(Types) == sizeof...(Hashers), "size mismatch");

    typedef std::tuple<Types...> argument_type;
    typedef std::tuple<Hashers...> hashers_type;
    typedef size_t result_type;

    tuple_hash()
    {}

    tuple_hash(Hashers&& ...hashers)
        : hashers(std::move(hashers)...)
    {}

    size_t operator()(const argument_type& arg) const
    {
        return tuple_hash_impl<argument_type, hashers_type, 0, std::tuple_size<argument_type>::value>::do_hash(arg, hashers);
    }

private:
    hashers_type hashers;
};

#endif
