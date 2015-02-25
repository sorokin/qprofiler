#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>

struct MyItem;

struct profile
{
    profile();

    void open(std::string const& filename);
    void build_tree(MyItem*);
    void build_reverse_tree(MyItem*);

private:
    struct backtrace
    {
        backtrace(std::vector<std::string> frames);

        std::vector<std::string> frames;
    };

    std::vector<backtrace> samples;
};

#endif // PROFILE_H
