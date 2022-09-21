#ifndef CHESSUCIENGINE_BITBOARD_H
#define CHESSUCIENGINE_BITBOARD_H

#include <cstdint>
#include "../utils.h"
#include <bit>
#include <bitset>
#include <ostream>

using bitboard = uint64_t;

inline constexpr void set_1(bitboard& board, uint8_t index) {
    board |= 1ULL << index;
}

inline constexpr void set_0(bitboard& board, uint8_t index) {
    board &= ~(1ULL << index);
}

inline constexpr bool get_bit(const bitboard& board, uint8_t index) {
    return board & (1ULL << index);
}

inline uint8_t lsb(const bitboard& board) {
    unsigned long index;
    Assert(_BitScanForward64(&index, board))
    return index;
}

inline uint8_t msb(const bitboard& board) {
    unsigned long index;
    Assert(_BitScanReverse64(&index, board))
    return index;
}

inline constexpr uint8_t count_1(const bitboard& board) {
    return std::popcount(board);
}

inline void print_bitboard(std::ostream& stream, const bitboard& board) {
    auto s = std::bitset<64>(board).to_string();
    std::reverse(s.begin(), s.end());
    for (int i = 7; i >= 0; i--) {
        auto ss = s.substr(i * 8, 8);
        stream << ss << std::endl;
    }
    stream << std::endl;
} 

#endif //CHESSUCIENGINE_BITBOARD_H
