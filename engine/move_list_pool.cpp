#include "move_list_pool.h"

move_list& move_list_pool::init_list(int index) {
    auto& list = pool[index];
    list.clear();
    return list;
}
