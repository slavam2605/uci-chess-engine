#include "move_list.h"

move_list::move_list() : _size(0) {}

chess_move& move_list::operator[](uint8_t index) {
    return moves[index];
}

const chess_move& move_list::operator[](uint8_t index) const {
    return moves[index];
}

void move_list::push_back(const chess_move& move) {
    moves[_size] = move;
    _size++;
}

uint8_t move_list::size() const {
    return _size;
}

array_type::iterator move_list::begin() {
    return moves.begin();
}

array_type::iterator move_list::end() {
    return moves.begin() + _size;
}

array_type::const_iterator move_list::begin() const {
    return moves.begin();
}

array_type::const_iterator move_list::end() const {
    return moves.begin() + _size;
}

void move_list::clear() {
    _size = 0;
}

void move_list::resize(size_t new_size) {
    Assert(new_size <= _size)
    _size = new_size;
}
