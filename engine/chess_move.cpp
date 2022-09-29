#include "chess_move.h"
#include "chess_utils.h"
#include <sstream>

using namespace std;

namespace move {
    const chess_move Invalid = make_move(chess::Empty, chess::Empty, chess::EmptyPiece, chess::EmptyPiece);

    chess_move make_move(uint8_t from, uint8_t to, uint8_t attacker_type, uint8_t defender_type, move_flag flag) {
        return from + (to << 8) + (static_cast<uint8_t>(flag) << 16) + (attacker_type << 20) + (defender_type << 23);
    }

    chess_move make_move(const chess_move& move, move_flag new_flag) {
        return (move & ~move::FlagMask) + (static_cast<uint8_t>(new_flag) << 16);
    }

    uint8_t from(const chess_move& move) {
        return move;
    }

    uint8_t to(const chess_move& move) {
        return move >> 8;
    }

    move_flag flag(const chess_move& move) {
        return static_cast<move_flag>((move >> 16) & 0xF);
    }

    uint8_t attacker(const chess_move& move) {
        return (move >> 20) & 0x7;
    }

    uint8_t defender(const chess_move& move) {
        return (move >> 23) & 0x7;
    }

    bool is_valid(const chess_move& move) {
        return move != Invalid;
    }

    string to_string(const chess_move& move) {
        stringstream ss;
        int from_row = move::from(move) / 8;
        int from_col = move::from(move) % 8;
        int to_row = move::to(move) / 8;
        int to_col = move::to(move) % 8;
        ss << char('a' + from_col) << char('1' + from_row) <<
           char('a' + to_col) << char('1' + to_row);
        switch (move::flag(move)) {
            case move::move_flag::PromoteToBishop: ss << 'b'; break;
            case move::move_flag::PromoteToKnight: ss << 'n'; break;
            case move::move_flag::PromoteToQueen: ss << 'q'; break;
            case move::move_flag::PromoteToRook: ss << 'r'; break;
            default: break;
        }
        return ss.str();
    }
}
