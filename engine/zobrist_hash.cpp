#include "zobrist_hash.h"
#include "../utils.h"

zobrist_hash::zobrist_hash() : value(0) {}

void zobrist_hash::invert_piece(int index, int side, int type) {
    value ^= zobrist_hash_utils::constants[index][side][type];
}

void zobrist_hash::invert_move() {
    value ^= zobrist_hash_utils::black_move;
}

void zobrist_hash::invert_castling(uint8_t side, uint8_t type) {
    uint8_t index = side * 2 + type;
    switch (index) {
        case 0: value ^= zobrist_hash_utils::black_long_castling; break;
        case 1: value ^= zobrist_hash_utils::black_short_castling; break;
        case 2: value ^= zobrist_hash_utils::white_long_castling; break;
        case 3: value ^= zobrist_hash_utils::white_short_castling; break;
        default: Assert(false)
    }
}