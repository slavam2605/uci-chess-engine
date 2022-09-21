#ifndef CHESSUCIENGINE_MAGIC_GENERATOR_H
#define CHESSUCIENGINE_MAGIC_GENERATOR_H

#include <random>
#include <array>
#include "magic_numbers.h"
#include "../bitboard.h"

namespace magic_generator {
    static std::random_device dev;
    static std::uniform_int_distribution<std::mt19937::result_type> dist;
    static uint64_t random_uint64(std::mt19937& rng);
    static uint64_t random_uint64_fewbits(std::mt19937& rng);
    static uint64_t find_magic(std::mt19937& rng, uint8_t square, uint8_t shift, bool is_bishop);
    void generate();

    static constexpr uint64_t get_rook_mask(int sq) {
        uint64_t result = 0ULL;
        int rk = sq/8, fl = sq%8, r, f;
        for(r = rk+1; r <= 6; r++) result |= (1ULL << (fl + r*8));
        for(r = rk-1; r >= 1; r--) result |= (1ULL << (fl + r*8));
        for(f = fl+1; f <= 6; f++) result |= (1ULL << (f + rk*8));
        for(f = fl-1; f >= 1; f--) result |= (1ULL << (f + rk*8));
        return result;
    }

    static constexpr uint64_t get_bishop_mask(int sq) {
        uint64_t result = 0ULL;
        int rk = sq/8, fl = sq%8, r, f;
        for(r=rk+1, f=fl+1; r<=6 && f<=6; r++, f++) result |= (1ULL << (f + r*8));
        for(r=rk+1, f=fl-1; r<=6 && f>=1; r++, f--) result |= (1ULL << (f + r*8));
        for(r=rk-1, f=fl+1; r>=1 && f<=6; r--, f++) result |= (1ULL << (f + r*8));
        for(r=rk-1, f=fl-1; r>=1 && f>=1; r--, f--) result |= (1ULL << (f + r*8));
        return result;
    }

    static constexpr uint64_t rook_attack(int sq, uint64_t block) {
        uint64_t result = 0ULL;
        int rk = sq/8, fl = sq%8, r, f;
        for(r = rk+1; r <= 7; r++) {
            result |= (1ULL << (fl + r*8));
            if(block & (1ULL << (fl + r*8))) break;
        }
        for(r = rk-1; r >= 0; r--) {
            result |= (1ULL << (fl + r*8));
            if(block & (1ULL << (fl + r*8))) break;
        }
        for(f = fl+1; f <= 7; f++) {
            result |= (1ULL << (f + rk*8));
            if(block & (1ULL << (f + rk*8))) break;
        }
        for(f = fl-1; f >= 0; f--) {
            result |= (1ULL << (f + rk*8));
            if(block & (1ULL << (f + rk*8))) break;
        }
        return result;
    }

    static constexpr uint64_t bishop_attack(int sq, uint64_t block) {
        uint64_t result = 0ULL;
        int rk = sq/8, fl = sq%8, r, f;
        for(r = rk+1, f = fl+1; r <= 7 && f <= 7; r++, f++) {
            result |= (1ULL << (f + r*8));
            if(block & (1ULL << (f + r * 8))) break;
        }
        for(r = rk+1, f = fl-1; r <= 7 && f >= 0; r++, f--) {
            result |= (1ULL << (f + r*8));
            if(block & (1ULL << (f + r * 8))) break;
        }
        for(r = rk-1, f = fl+1; r >= 0 && f <= 7; r--, f++) {
            result |= (1ULL << (f + r*8));
            if(block & (1ULL << (f + r * 8))) break;
        }
        for(r = rk-1, f = fl-1; r >= 0 && f >= 0; r--, f--) {
            result |= (1ULL << (f + r*8));
            if(block & (1ULL << (f + r * 8))) break;
        }
        return result;
    }
    
    static uint64_t index_to_mask(uint64_t mask, uint64_t index) {
        uint64_t result = 0;
        while (mask) {
            uint8_t bit = lsb(mask);
            set_0(mask, bit);
            result |= (index & 1) << bit;
            index >>= 1;
        }
        return result;
    }

    static void calc_bishop_attack_masks(std::array<std::array<uint64_t, 512>, 64>& result) {
        for (uint8_t square = 0; square < 64; square++) {
            auto mask = get_bishop_mask(square);
            auto magic = magic::bishop_magic[square];
            auto shift = magic::bishop_shift[square];
            uint8_t n = count_1(mask);
            for (int i = 0; i < (1 << n); i++) {
                uint64_t occ = index_to_mask(mask, i);
                int index = (int) ((occ * magic) >> (64 - shift));
                result[square][index] = bishop_attack(square, occ);
            }
        }
    }
    
    static void calc_rook_attack_masks(std::array<std::array<uint64_t, 4096>, 64>& result) {
        for (uint8_t square = 0; square < 64; square++) {
            auto mask = get_rook_mask(square);
            auto magic = magic::rook_magic[square];
            auto shift = magic::rook_shift[square];
            uint8_t n = count_1(mask);
            for (int i = 0; i < (1 << n); i++) {
                uint64_t occ = index_to_mask(mask, i);
                int index = (int) ((occ * magic) >> (64 - shift));
                result[square][index] = rook_attack(square, occ);
            }
        }
    }
    
    static consteval std::array<uint64_t, 64> calc_bishop_mask() {
        std::array<uint64_t, 64> result{};
        for (int i = 0; i < 64; i++) {
            result[i] = get_bishop_mask(i);
        }
        return result;
    }

    static consteval std::array<uint64_t, 64> calc_rook_mask() {
        std::array<uint64_t, 64> result{};
        for (int i = 0; i < 64; i++) {
            result[i] = get_rook_mask(i);
        }
        return result;
    }
    
    static constexpr std::array<uint64_t, 64> bishop_mask = calc_bishop_mask();
    static constexpr std::array<uint64_t, 64> rook_mask = calc_rook_mask();

    inline std::array<std::array<uint64_t, 512>, 64> bishop_attack_masks;
    inline std::array<std::array<uint64_t, 4096>, 64> rook_attack_masks;
    static bool is_initialized = false;
    
    static void init() {
        if (is_initialized) return;
        calc_bishop_attack_masks(bishop_attack_masks);
        calc_rook_attack_masks(rook_attack_masks);
        is_initialized = true;
    }
};

struct [[maybe_unused]] magic_initializer {
    [[maybe_unused]] static magic_initializer Instance;
    magic_initializer() {
        magic_generator::init();
    }
};


#endif //CHESSUCIENGINE_MAGIC_GENERATOR_H
