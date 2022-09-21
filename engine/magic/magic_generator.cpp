#include "magic_generator.h"
#include "../bitboard.h"
#include "magic_numbers.h"
#include <cstdint>
#include <iostream>
#include <iomanip>

using namespace std;

uint64_t magic_generator::random_uint64(mt19937& rng) {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(dist(rng)) & 0xFFFF; u2 = (uint64_t)(dist(rng)) & 0xFFFF;
    u3 = (uint64_t)(dist(rng)) & 0xFFFF; u4 = (uint64_t)(dist(rng)) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t magic_generator::random_uint64_fewbits(mt19937& rng) {
    return random_uint64(rng) & random_uint64(rng) & random_uint64(rng);
}

uint64_t magic_generator::find_magic(mt19937& rng, uint8_t square, uint8_t shift, bool is_bishop) {
    uint64_t occ[4096], att[4096], used[4096], magic;
    uint64_t mask = is_bishop ? get_bishop_mask(square) : get_rook_mask(square);
    uint8_t n = count_1(mask);
    
    for (int i = 0; i < (1 << n); i++) {
        occ[i] = index_to_mask(mask, i);
        att[i] = is_bishop ? bishop_attack(square, occ[i]) : rook_attack(square, occ[i]);
    }
    
    for (int _ = 0; _ < 100000000; _++) {
        bool failed = false;
        magic = random_uint64_fewbits(rng);
        if (count_1((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
        for (auto& value : used) value = 0;
        for (int i = 0; i < (1 << n); i++) {
            int j = (int) ((occ[i] * magic) >> (64 - shift));
            if (used[j] == 0) {
                used[j] = att[i];
            } else if (used[j] != att[i]) {
                failed = true;
                break;
            }
        }
        if (!failed) return magic;
    }
    cerr << "Failed to find magic number for square " << square << " for " << (is_bishop ? "bishop" : "rook") << endl;
    return 0;
}

[[maybe_unused]] magic_initializer magic_initializer::Instance;

void magic_generator::generate() {
    mt19937 rng(dev());

    cout << hex;
    cout << "static constexpr uint64_t rook_magic[64] = {" << endl;
    for (int square = 0; square < 64; square++)
        cout << "  0x" << find_magic(rng, square, magic::rook_shift[square], false) << "ULL," << endl;
    cout << "};" << endl << endl;
    
    cout << "static constexpr uint64_t bishop_magic[64] = {" << endl;
    for (int square = 0; square < 64; square++)
        cout << "  0x" << find_magic(rng, square, magic::bishop_shift[square], true) << "ULL," << endl;
    cout << "};" << endl << endl;
}
