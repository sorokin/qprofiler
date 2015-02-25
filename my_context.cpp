#include "my_context.h"
#include "my_item.h"
#include <cassert>

MyContext::MyContext()
{}

void MyContext::set_root(MyItem* item)
{
    root = item;
}

MyItem* MyContext::get_root()
{
    return root;
}

size_t MyContext::total_hits()
{
    assert(root);
    return root->hits();
}

