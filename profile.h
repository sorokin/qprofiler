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
    void build_tree(MyItem*);
    void build_reverse_tree(MyItem*);

private:
    struct frame
    {
        frame(string_ref function_name, string_ref dso_name);

        string_ref function_name;
        string_ref dso_name;
    };

    struct backtrace
    {
        backtrace(std::vector<frame> frames);

        std::vector<frame> frames;
    };

    string_pool function_names;
    string_pool dso_names;
    std::vector<backtrace> samples;
};

#endif // PROFILE_H
