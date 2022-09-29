#include "chess_move.h"
#include "chess_utils.h"

// This constructor leaves fields uninitialized intentionally to improve performance
chess_move::chess_move() {} // NOLINT(cppcoreguidelines-pro-type-member-init,modernize-use-equals-default)

const chess_move chess_move::Invalid = {chess::Empty, chess::Empty, chess::Empty, chess::Empty};

chess_move::chess_move(uint8_t from, uint8_t to, uint8_t attacker_type, uint8_t defender_type, chess_move::move_flag flag)
        : packed(from + (to << 8) + (attacker_type << 16) + (defender_type << 24)), flag(flag), evaluation(0) {}

chess_move::chess_move(const chess_move& move, chess_move::move_flag new_flag) : chess_move(move) {
    flag = new_flag;
}
        
bool operator==(const chess_move& lhs, const chess_move& rhs) {
    // TODO[move] bitwise comparison or even default
    return move::from(lhs) == move::from(rhs) && move::to(lhs) == move::to(rhs) && lhs.flag == rhs.flag;
}

std::ostream& operator<<(std::ostream& stream, const chess_move& move) {
    int from_row = move::from(move) / 8;
    int from_col = move::from(move) % 8;
    int to_row = move::to(move) / 8;
    int to_col = move::to(move) % 8;
    stream << char('a' + from_col) << char('1' + from_row) <<
           char('a' + to_col) << char('1' + to_row);
    switch (move.flag) {
        case chess_move::move_flag::PromoteToBishop: stream << 'b'; break;
        case chess_move::move_flag::PromoteToKnight: stream << 'n'; break;
        case chess_move::move_flag::PromoteToQueen: stream << 'q'; break;
        case chess_move::move_flag::PromoteToRook: stream << 'r'; break;
        default: break;
    }
    return stream;
}

bool chess_move::is_valid() const {
    // TODO[move] check for InvalidMove or somehow else
    return move::from(*this) != move::to(*this);
}

namespace move {
    uint8_t from(const chess_move& move) {
        return move.packed & 0xFF;
    }

    uint8_t to(const chess_move& move) {
        return (move.packed >> 8) & 0xFF;
    }

    uint8_t attacker(const chess_move& move) {
        return (move.packed >> 16) & 0xFF;
    }

    uint8_t defender(const chess_move& move) {
        return (move.packed >> 24) & 0xFF;
    }
}
