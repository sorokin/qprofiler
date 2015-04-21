#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>
#include "string_pool.h"
#include "object_pool.h"

struct MyItem;

struct profile
{
    struct frame
    {
        frame(string_ref function_name, string_ref dso_name);

        string_ref function_name;
        string_ref dso_name;
    };

    typedef size_t frame_index_underlying_type;
    typedef object_pool<frame, frame_index_underlying_type> frame_pool_type;
    typedef typename frame_pool_type::index_type frame_index_type;

    profile();

    void open(std::string const& filename);
    void build_tree(MyItem*);
    void build_reverse_tree(MyItem*);

    frame const& get_frame(frame_index_type index);

private:
    struct backtrace
    {
        backtrace(std::vector<frame_index_type> frames);

        std::vector<frame_index_type> frames;
    };

    string_pool function_names;
    string_pool dso_names;
    frame_pool_type frame_pool;
    std::vector<backtrace> samples;
};

#endif // PROFILE_H
