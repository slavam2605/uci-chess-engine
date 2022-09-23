#ifndef CHESSUCIENGINE_BITBOARD_UTILS_H
#define CHESSUCIENGINE_BITBOARD_UTILS_H

#include "move_masks.h"

inline bool aligned(uint8_t s1, uint8_t s2, uint8_t s3) {
    return line_masks::mask[s1][s2] & (1ULL << s3);
}

#endif //CHESSUCIENGINE_BITBOARD_UTILS_H
