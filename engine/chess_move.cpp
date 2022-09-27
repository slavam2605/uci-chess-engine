#include "chess_move.h"
#include "chess_utils.h"

// This constructor leaves fields uninitialized intentionally to improve performance
chess_move::chess_move() {} // NOLINT(cppcoreguidelines-pro-type-member-init,modernize-use-equals-default)

chess_move chess_move::invalid() {
    return {chess::Empty, chess::Empty, chess::Empty, chess::Empty, chess::Empty};
}

chess_move::chess_move(uint8_t from, uint8_t to, uint8_t side, uint8_t attacker_type, uint8_t defender_type, chess_move::move_flag flag)
        : from(from), to(to), side(side), attacker_type(attacker_type), defender_type(defender_type), flag(flag) {}

chess_move::chess_move(const chess_move& move, chess_move::move_flag new_flag) : chess_move(move) {
    flag = new_flag;
}
        
bool operator==(const chess_move& lhs, const chess_move& rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to && lhs.flag == rhs.flag;
}

std::ostream& operator<<(std::ostream& stream, const chess_move& move) {
    int from_row = move.from / 8;
    int from_col = move.from % 8;
    int to_row = move.to / 8;
    int to_col = move.to % 8;
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
    return from != chess::Empty;
}

