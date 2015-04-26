#ifndef MY_CONTEXT_H
#define MY_CONTEXT_H

#include <cstdlib>

struct my_item;

struct MyContext
{
    MyContext();
    void set_root(my_item* item);
    my_item* get_root();
    size_t total_hits();

private:
    my_item* root;
};

#endif // MY_CONTEXT_H
