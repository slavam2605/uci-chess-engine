#ifndef CHESSUCIENGINE_MOVE_MASKS_H
#define CHESSUCIENGINE_MOVE_MASKS_H

#include <array>
#include <functional>
#include "bitboard.h"

static consteval uint8_t abs_sub(uint8_t left, uint8_t right) {
    return left > right ? left - right : right - left;
}

template <size_t Size>
static consteval std::array<bitboard , Size> inv_bitboard_array(const std::array<bitboard, Size>& array) {
    std::array<bitboard, Size> result{};
    for (int i = 0; i < Size; i++) {
        result[i] = ~array[i];
    }
    return result;
}

namespace util_mask {
    static consteval std::array<bitboard, 8> calc_rows() {
        std::array<bitboard, 8> result{};
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 8; col++) {
                set_1(result[row], row * 8 + col);
            }
        }
        return result;
    }

    static consteval std::array<bitboard, 8> calc_cols() {
        std::array<bitboard, 8> result{};
        for (uint8_t col = 0; col < 8; col++) {
            for (uint8_t row = 0; row < 8; row++) {
                set_1(result[col], row * 8 + col);
            }
        }
        return result;
    }
    
    static constexpr std::array<bitboard, 8> rows = calc_rows();
    static constexpr std::array<bitboard, 8> cols = calc_cols();
    static constexpr std::array<bitboard, 8> inv_rows = inv_bitboard_array(rows);
    static constexpr std::array<bitboard, 8> inv_cols = inv_bitboard_array(cols);
}

namespace king_mask {
    static consteval std::array<bitboard, 64> calc_masks() {
        std::array<bitboard, 64> result{};
        for (uint8_t from_col = 0; from_col < 8; from_col++) {
            for (uint8_t from_row = 0; from_row < 8; from_row++) {
                uint8_t from = from_row * 8 + from_col;
                for (uint8_t to_col = 0; to_col < 8; to_col++) {
                    for (uint8_t to_row = 0; to_row < 8; to_row++) {
                        uint8_t to = to_row * 8 + to_col;
                        
                        uint8_t dx = abs_sub(from_col, to_col);
                        uint8_t dy = abs_sub(from_row, to_row);
                        if (dx <= 1 && dy <= 1) {
                            set_1(result[from], to);
                        }
                    }
                }
            }
        }
        return result;
    }
    
    static constexpr std::array<bitboard, 64> mask = calc_masks();
}

namespace knight_mask {
    static consteval std::array<bitboard, 64> calc_masks() {
        std::array<bitboard, 64> result{};
        for (uint8_t from_col = 0; from_col < 8; from_col++) {
            for (uint8_t from_row = 0; from_row < 8; from_row++) {
                uint8_t from = from_row * 8 + from_col;
                for (uint8_t to_col = 0; to_col < 8; to_col++) {
                    for (uint8_t to_row = 0; to_row < 8; to_row++) {
                        uint8_t to = to_row * 8 + to_col;

                        uint8_t dx = abs_sub(from_col, to_col);
                        uint8_t dy = abs_sub(from_row, to_row);
                        if (dx == 1 && dy == 2 || dx == 2 && dy == 1) {
                            set_1(result[from], to);
                        }
                    }
                }
            }
        }
        return result;
    }

    static constexpr std::array<bitboard, 64> mask = calc_masks();
}

namespace slider_mask {
    struct direction {
        static constexpr uint8_t North = 0;
        static constexpr uint8_t South = 1;
        static constexpr uint8_t West = 2;
        static constexpr uint8_t East = 3;
        static constexpr uint8_t NorthWest = 4;
        static constexpr uint8_t NorthEast = 5;
        static constexpr uint8_t SouthWest = 6;
        static constexpr uint8_t SouthEast = 7;
    };


    static consteval bitboard calc_mask(uint8_t p, uint8_t direction) {
        bitboard mask = 0;
        auto x = static_cast<int8_t>(p % 8);
        auto y = static_cast<int8_t>(p / 8);

        for (;;) {
            switch (direction) {
                case direction::North: y++; break;
                case direction::South: y--; break;
                case direction::West: x--; break;
                case direction::East: x++; break;
                case direction::NorthWest: y++; x--; break;
                case direction::NorthEast: y++; x++; break;
                case direction::SouthWest: y--; x--; break;
                case direction::SouthEast: y--; x++; break;
                default: Assert(false)
            }
            if (x > 7 or x < 0 or y > 7 or y < 0) break;
            set_1(mask, y * 8 + x);
        }

        return mask;
    }

    static consteval std::array<std::array<bitboard, 8>, 64> calc_mask() {
        std::array<std::array<bitboard, 8>, 64> masks{};
        for (uint8_t i = 0; i < 64; i++) {
            for (uint8_t j = 0; j < 8; j++) masks[i][j] = calc_mask(i, j);
        }
        return masks;
    }

    static constexpr std::array<std::array<bitboard, 8>, 64> mask = calc_mask();
};

namespace passed_pawn_masks {
    static consteval std::array<bitboard, 64> calc_white_passed_pawn_masks() {
        std::array<bitboard, 64> masks{};
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                for (int y1 = y + 1; y1 < 8; y1++) {
                    if (x != 0) set_1(masks[y * 8 + x], y1 * 8 + x - 1);
                    if (x != 7) set_1(masks[y * 8 + x], y1 * 8 + x + 1);
                    set_1(masks[y * 8 + x], y1 * 8 + x);
                }
            }
        }
        return masks;
    }


    static consteval std::array<bitboard, 64> calc_black_passed_pawn_masks() {
        std::array<bitboard, 64> masks{};
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                for (int y1 = y - 1; y1 >= 0; y1--) {
                    if (x != 0) set_1(masks[y * 8 + x], y1 * 8 + x - 1);
                    if (x != 7) set_1(masks[y * 8 + x], y1 * 8 + x + 1);
                    set_1(masks[y * 8 + x], y1 * 8 + x);
                }
            }
        }
        return masks;
    }


    static constexpr std::array<bitboard, 64> white_mask = calc_white_passed_pawn_masks();
    static constexpr std::array<bitboard, 64> black_mask = calc_black_passed_pawn_masks();
}

namespace pawn_shield_mask {
    static consteval std::array<bitboard, 64> calc_white_mask() {
        std::array<bitboard, 64> white_pawn_shield_masks{};

        for (uint8_t x = 0; x < 8; x = x + 1) {
            for (uint8_t y = 0; y < 7; y = y + 1) {
                if (x != 0) set_1(white_pawn_shield_masks[y * 8 + x], (y + 1) * 8 + x - 1);
                if (x != 7) set_1(white_pawn_shield_masks[y * 8 + x], (y + 1) * 8 + x + 1);
                set_1(white_pawn_shield_masks[y * 8 + x], (y + 1) * 8 + x);

                if (y != 6) {
                    if (x != 0) set_1(white_pawn_shield_masks[y * 8 + x], (y + 2) * 8 + x - 1);
                    if (x != 7) set_1(white_pawn_shield_masks[y * 8 + x], (y + 2) * 8 + x + 1);
                    set_1(white_pawn_shield_masks[y * 8 + x], (y + 2) * 8 + x);
                }
            }
        }

        return white_pawn_shield_masks;
    }


    static consteval std::array<bitboard, 64> calc_black_mask() {
        std::array<bitboard, 64> black_pawn_shield_masks{};

        for (uint8_t x = 0; x < 8; x = x + 1) {
            for (uint8_t y = 1; y < 8; y = y + 1) {
                if (x != 0) set_1(black_pawn_shield_masks[y * 8 + x], (y - 1) * 8 + x - 1);
                if (x != 7) set_1(black_pawn_shield_masks[y * 8 + x], (y - 1) * 8 + x + 1);
                set_1(black_pawn_shield_masks[y * 8 + x], (y - 1) * 8 + x);

                if (y != 1) {
                    if (x != 0) set_1(black_pawn_shield_masks[y * 8 + x], (y - 2) * 8 + x - 1);
                    if (x != 7) set_1(black_pawn_shield_masks[y * 8 + x], (y - 2) * 8 + x + 1);
                    set_1(black_pawn_shield_masks[y * 8 + x], (y - 2) * 8 + x);
                }
            }
        }

        return black_pawn_shield_masks;
    }


    static constexpr std::array<bitboard, 64> white_mask = calc_white_mask();
    static constexpr std::array<bitboard, 64> black_mask = calc_black_mask();
}

namespace in_between_mask {
    static consteval bitboard calc_in_between(uint8_t sq1, uint8_t sq2) {
        const auto m1   = uint64_t(-1);
        const auto a2a7 = 0x0001010101010100ULL;
        const auto b2g7 = 0x0040201008040200ULL;
        const auto h1b7 = 0x0002040810204080ULL;
        uint64_t btwn, line, rank, file;

        btwn  = (m1 << sq1) ^ (m1 << sq2);
        file  =   (sq2 & 7) - (sq1   & 7);
        rank  =  ((sq2 | 7) -  sq1) >> 3 ;
        line  =      (   (file  &  7) - 1) & a2a7;
        line += 2 * ((   (rank  &  7) - 1) >> 58);
        line += (((rank - file) & 15) - 1) & b2g7;
        line += (((rank + file) & 15) - 1) & h1b7;
        line *= btwn & -btwn;
        return line & btwn;
    }
    
    static consteval std::array<std::array<bitboard, 64>, 64> calc_mask() {
        std::array<std::array<bitboard, 64>, 64> result{};
        for (uint8_t from = 0; from < 64; from++) {
            for (uint8_t to = 0; to < 64; to++) {
                result[from][to] = calc_in_between(from, to);
            }
        }
        return result;
    }
    
    static constexpr std::array<std::array<bitboard, 64>, 64> mask = calc_mask();
}

#endif //CHESSUCIENGINE_MOVE_MASKS_H
