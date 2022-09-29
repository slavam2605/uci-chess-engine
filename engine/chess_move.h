#ifndef CHESSUCIENGINE_CHESS_MOVE_H
#define CHESSUCIENGINE_CHESS_MOVE_H

#include "../utils.h"
#include <cstdint>
#include <ostream>

struct chess_move {
    enum class move_flag : uint8_t {
        Default, PawnLongMove, EnPassantCapture, 
        WhiteLongCastling, WhiteShortCastling,
        BlackLongCastling, BlackShortCastling,
        PromoteToKnight, PromoteToBishop,
        PromoteToRook, PromoteToQueen
    };

    static const chess_move Invalid;
    static constexpr uint32_t FlagMask = 0xF0000;

    /**
     * [0-7 bits] from
     * [8-15 bits] to
     * [16-19 bits] flag
     * [20-22 bits] attacker
     * [23-25 bits] defender
     */
    uint32_t packed;
    int evaluation;
    
    chess_move();
    chess_move(uint8_t from, uint8_t to, uint8_t attacker_type, uint8_t defender_type, move_flag flag = move_flag::Default);
    chess_move(const chess_move& move) = default;
    chess_move(const chess_move& move, move_flag new_flag);
    [[nodiscard]] bool is_valid() const;

    friend bool operator==(const chess_move& lhs, const chess_move& rhs);
    friend std::ostream& operator<<(std::ostream& stream, const chess_move& move);
};

namespace move {
    uint8_t from(const chess_move& move);
    uint8_t to(const chess_move& move);
    chess_move::move_flag flag(const chess_move& move);
    uint8_t attacker(const chess_move& move);
    uint8_t defender(const chess_move& move);
}

namespace std {
    template <>
    struct hash<chess_move> {
        size_t operator()(const chess_move& move) const {
            return move.packed;
        }
    };
}

#endif //CHESSUCIENGINE_CHESS_MOVE_H
