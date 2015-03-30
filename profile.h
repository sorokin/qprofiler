#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>
#include "string_pool.h"

struct MyItem;

struct profile
{
    profile();

    void open(std::string const& filename);
    void open_perf_data(std::string const& filename);
    void build_tree(MyItem*);
    void build_reverse_tree(MyItem*);

private:
    struct backtrace
    {
        backtrace(std::vector<string_ref> frames);

        std::vector<string_ref> frames;
    };

    string_pool frame_names;
    std::vector<backtrace> samples;
};

#endif // PROFILE_H
