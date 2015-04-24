#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tuple_hash.h"
#include "tuple_equals.h"
#include "type_list.h"

template <typename T, typename Index>
struct object_pool;

template <typename T, typename Index>
struct object_pool_index_hash;

template <typename T, typename Index, typename Args, typename Hashers, typename Comparers>
struct object_pool_builder;

template <typename T, typename Index>
struct object_pool
{
    static_assert(std::is_unsigned<Index>::value, "index must be unsigned");

    struct index_type
    {
        typedef object_pool_index_hash<T, Index> hash;

        explicit constexpr index_type(Index val)
            : val(val)
        {}

        Index value() const
        {
            return val;
        }

        friend bool operator==(index_type const& a, index_type const& b)
        {
            return a.val == b.val;
        }

        friend bool operator!=(index_type const& a, index_type const& b)
        {
            return a.val != b.val;
        }

    private:
        Index val;
    };

    typedef T value_type;
    template <typename Args, typename Hashers, typename Comparers>
    using builder = object_pool_builder<T, Index, Args, Hashers, Comparers>;

    object_pool()
    {}

    explicit object_pool(std::size_t capacity)
    {
        objects.reserve(capacity);
    }

    index_type put(T&& obj)
    {
        objects.push_back(std::move(obj));
        return index_type(objects.size() - 1);
    }

    index_type put(T const& obj)
    {
        objects.push_back(obj);
        return index_type(objects.size() - 1);
    }

    template <typename... Args>
    index_type emplace(Args&& ...args)
    {
        objects.emplace_back(args...);
        return index_type(objects.size() - 1);
    }

    T& operator[](index_type i) const
    {
        return objects[i.value()];
    }

private:
    mutable std::vector<T> objects;
};

template <typename T, typename Index, typename... Args, typename... Hashers, typename... Comparers>
struct object_pool_builder<T, Index, type_list<Args...>, type_list<Hashers...>, type_list<Comparers...>>
{
    static_assert(std::is_unsigned<Index>::value, "index must be unsigned");

    typedef T value_type;
    typedef object_pool<T, Index> object_pool_type;
    typedef typename object_pool_type::index_type index_type;

    object_pool_builder(object_pool_type& pool)
        : pool(pool)
    {}

    index_type emplace(Args&& ...args)
    {
        key_type key(args...);
        auto i = mapping.find(key);
        if (i != mapping.end())
        {
            return i->second;
        }
        else
        {
            index_type index = pool.emplace(args...);
            mapping.insert(std::make_pair(key, index));
            return index;
        }
    }

private:
    typedef std::tuple<Args...> key_type;
    typedef tuple_hash<type_list<Args...>, type_list<Hashers...>> hasher_type;
    typedef tuple_equals<type_list<Args...>, type_list<Comparers...>> comparer_type;

    object_pool_type& pool;
    std::unordered_map<key_type, index_type, hasher_type, comparer_type> mapping;
};

template <typename T, typename Index>
struct object_pool_index_hash
{
    size_t operator()(typename object_pool<T, Index>::index_type index) const
    {
        return std::hash<Index>()(index.value());
    }
};

#endif
