#ifndef CHESSUCIENGINE_MOVE_LIST_POOL_H
#define CHESSUCIENGINE_MOVE_LIST_POOL_H

#include <array>
#include "move_list.h"

class move_list_pool {
    std::array<move_list, 250> pool{};
public:
    move_list& init_list(int index);
};


#endif //CHESSUCIENGINE_MOVE_LIST_POOL_H
