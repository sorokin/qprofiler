#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <unordered_set>

#include "profile.h"

struct transformation
{
    enum class direction_type
    {
        forward,
        backward,
    };

    direction_type direction = direction_type::forward;
    std::unordered_set<profile::frame_index_type, profile::frame_index_type::hash> roots;
};

#endif
