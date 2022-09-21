#ifndef CHESSUCIENGINE_MOVE_LIST_H
#define CHESSUCIENGINE_MOVE_LIST_H

#include <array>
#include "chess_move.h"

using array_type = std::array<chess_move, 218>;

class move_list {
    array_type moves{};
    uint8_t _size;
public:
    move_list();
    chess_move& operator[](uint8_t index);
    const chess_move& operator[](uint8_t index) const;
    void push_back(const chess_move& move);
    [[nodiscard]] uint8_t size() const;
    void clear();
    array_type::iterator begin();
    array_type::iterator end();
};


#endif //CHESSUCIENGINE_MOVE_LIST_H
