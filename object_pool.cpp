#include "object_pool.h"

template struct object_pool<int, size_t>;
template struct object_pool_builder<int, size_t, type_list<int>, type_list<std::hash<int>>, type_list<std::equal_to<int>>>;
