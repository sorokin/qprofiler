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

    typedef std::unordered_set<profile::frame_index_type, profile::frame_index_type::hash> frames_set;
    typedef std::vector<frames_set> roots_type;

    direction_type direction = direction_type::forward;
    roots_type roots;
};

#endif
