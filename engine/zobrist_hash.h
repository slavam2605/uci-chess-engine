#ifndef CHESSUCIENGINE_ZOBRIST_HASH_H
#define CHESSUCIENGINE_ZOBRIST_HASH_H

#include <cstdint>
#include <array>

struct zobrist_hash {
    uint64_t value;
    
    zobrist_hash();
    zobrist_hash(const zobrist_hash& hash) = default;
    zobrist_hash& operator=(const zobrist_hash& hash) = default;
    
    void invert_piece(int index, int side, int type);
    void invert_move();
    void invert_castling(uint8_t side, uint8_t type);
};

namespace zobrist_hash_utils {
    namespace PRNG {
        static constexpr uint64_t Seed = 0x98f107;
        static constexpr uint64_t Multiplier = 0x71abc9;
        static constexpr uint64_t Summand = 0xff1b3f;
    }

    static consteval uint64_t next_random(uint64_t previous) {
        return PRNG::Multiplier * previous + PRNG::Summand;
    }

    static consteval std::array<std::array<std::array<uint64_t, 6>, 2>, 64> calc_constants() {
        std::array<std::array<std::array<uint64_t, 6>, 2>, 64> constants{};
        uint64_t previous = PRNG::Seed;
        for (uint8_t square = 0; square < 64; square = square + 1) {
            for (uint8_t side = 0; side < 2; side = side + 1) {
                for (uint8_t type = 0; type < 6; type = type + 1) {
                    previous = next_random(previous);
                    constants[square][side][type] = previous;
                }
            }
        }
        return constants;
    }

    static constexpr std::array<std::array<std::array<uint64_t, 6>, 2>, 64> constants = calc_constants();
    static constexpr uint64_t black_move = next_random(constants[63][1][5]);
    static constexpr uint64_t white_long_castling = next_random(black_move);
    static constexpr uint64_t white_short_castling = next_random(white_long_castling);
    static constexpr uint64_t black_long_castling = next_random(white_short_castling);
    static constexpr uint64_t black_short_castling = next_random(black_long_castling);
}


#endif //CHESSUCIENGINE_ZOBRIST_HASH_H
