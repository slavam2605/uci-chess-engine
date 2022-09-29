#include "chess_move.h"
#include "chess_utils.h"

// This constructor leaves fields uninitialized intentionally to improve performance
chess_move::chess_move() {} // NOLINT(cppcoreguidelines-pro-type-member-init,modernize-use-equals-default)

const chess_move chess_move::Invalid = {chess::Empty, chess::Empty, chess::EmptyPiece, chess::EmptyPiece};

chess_move::chess_move(uint8_t from, uint8_t to, uint8_t attacker_type, uint8_t defender_type, chess_move::move_flag flag)
        : packed(from + (to << 8) + (static_cast<uint8_t>(flag) << 16) + (attacker_type << 20) + (defender_type << 23)), evaluation(0) {}

chess_move::chess_move(const chess_move& move, chess_move::move_flag new_flag) : chess_move(move) {
    packed = (packed & ~FlagMask) + (static_cast<uint8_t>(new_flag) << 16);
}
        
bool operator==(const chess_move& lhs, const chess_move& rhs) {
    return lhs.packed == rhs.packed;
}

std::ostream& operator<<(std::ostream& stream, const chess_move& move) {
    int from_row = move::from(move) / 8;
    int from_col = move::from(move) % 8;
    int to_row = move::to(move) / 8;
    int to_col = move::to(move) % 8;
    stream << char('a' + from_col) << char('1' + from_row) <<
           char('a' + to_col) << char('1' + to_row);
    switch (move::flag(move)) {
        case chess_move::move_flag::PromoteToBishop: stream << 'b'; break;
        case chess_move::move_flag::PromoteToKnight: stream << 'n'; break;
        case chess_move::move_flag::PromoteToQueen: stream << 'q'; break;
        case chess_move::move_flag::PromoteToRook: stream << 'r'; break;
        default: break;
    }
    return stream;
}

bool chess_move::is_valid() const {
    return *this != Invalid;
}

namespace move {
    uint8_t from(const chess_move& move) {
        return move.packed;
    }

    uint8_t to(const chess_move& move) {
        return move.packed >> 8;
    }

    chess_move::move_flag flag(const chess_move& move) {
        return static_cast<chess_move::move_flag>((move.packed >> 16) & 0xF);
    }

    uint8_t attacker(const chess_move& move) {
        return (move.packed >> 20) & 0x7;
    }

    uint8_t defender(const chess_move& move) {
        return (move.packed >> 23) & 0x7;
    }
}
