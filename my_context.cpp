#include "my_context.h"
#include "my_item.h"
#include <cassert>

MyContext::MyContext()
{}

void MyContext::set_root(my_item* item)
{
    root = item;
}

my_item* MyContext::get_root()
{
    return root;
}

size_t MyContext::total_hits()
{
    assert(root);
    return root->hits();
}

