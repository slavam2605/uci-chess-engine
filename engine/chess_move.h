#ifndef CHESSUCIENGINE_CHESS_MOVE_H
#define CHESSUCIENGINE_CHESS_MOVE_H

#include "../utils.h"
#include <cstdint>
#include <ostream>

/**
 * [0-7 bits] from
 * [8-15 bits] to
 * [16-19 bits] flag
 * [20-22 bits] attacker
 * [23-25 bits] defender
 */
using chess_move = uint32_t;

namespace move {
    enum class move_flag : uint8_t {
        Default, PawnLongMove, EnPassantCapture,
        WhiteLongCastling, WhiteShortCastling,
        BlackLongCastling, BlackShortCastling,
        PromoteToKnight, PromoteToBishop,
        PromoteToRook, PromoteToQueen
    };

    extern const chess_move Invalid;
    static constexpr uint32_t FlagMask = 0xF0000;

    chess_move make_move(uint8_t from, uint8_t to, uint8_t attacker_type, uint8_t defender_type, move_flag flag = move_flag::Default);
    chess_move make_move(const chess_move& move, move_flag new_flag);
    uint8_t from(const chess_move& move);
    uint8_t to(const chess_move& move);
    move::move_flag flag(const chess_move& move);
    uint8_t attacker(const chess_move& move);
    uint8_t defender(const chess_move& move);
    bool is_valid(const chess_move& move);
    std::string to_string(const chess_move& move);
}

#endif //CHESSUCIENGINE_CHESS_MOVE_H
