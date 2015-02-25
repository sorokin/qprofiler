#ifndef MY_CONTEXT_H
#define MY_CONTEXT_H

#include <cstdlib>

struct MyItem;

struct MyContext
{
    MyContext();
    void set_root(MyItem* item);
    MyItem* get_root();
    size_t total_hits();

private:
    MyItem* root;
};

#endif // MY_CONTEXT_H
