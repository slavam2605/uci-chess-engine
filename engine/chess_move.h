#ifndef CHESSUCIENGINE_CHESS_MOVE_H
#define CHESSUCIENGINE_CHESS_MOVE_H

#include "../utils.h"
#include <cstdint>
#include <ostream>

struct chess_move {
    enum class move_flag {
        Default, PawnLongMove, EnPassantCapture, 
        WhiteLongCastling, WhiteShortCastling,
        BlackLongCastling, BlackShortCastling,
        PromoteToKnight, PromoteToBishop,
        PromoteToRook, PromoteToQueen
    };

    uint8_t from, to;
    move_flag flag;
    uint8_t side;
    uint8_t attacker_type;
    uint8_t defender_type;
    
    chess_move();
    chess_move(uint8_t from, uint8_t to, uint8_t side, uint8_t attacker_type, uint8_t defender_type, move_flag flag = move_flag::Default);
    chess_move(const chess_move& move) = default;
    chess_move(const chess_move& move, move_flag new_flag);
    static chess_move invalid();
    [[nodiscard]] bool is_valid() const;

    friend bool operator==(const chess_move& lhs, const chess_move& rhs);
    friend std::ostream& operator<<(std::ostream& stream, const chess_move& move);
};

MAKE_HASHABLE(chess_move, t.from, t.to, t.flag)

#endif //CHESSUCIENGINE_CHESS_MOVE_H
