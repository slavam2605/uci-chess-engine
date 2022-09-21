#include "game_state.h"
#include "../utils.h"
#include "chess_utils.h"
#include "chess_move_generator.h"
#include <cctype>
#include <sstream>

using namespace std;
using namespace chess;

string next_token(const string& s, const string& delim, uint64_t& pos) {
    auto pos_end = s.find(delim, pos);
    if (pos_end == string::npos) pos_end = s.length();
    string token = s.substr(pos, pos_end - pos);
    pos = pos_end + 1;
    return token;
}

game_state::game_state(const std::string& fen) {
    uint64_t pos = 0;
    string fen_board = next_token(fen, " ", pos);
    string active_color = next_token(fen, " ", pos);
    string available_castling = next_token(fen, " ", pos);
    string en_passant_square = next_token(fen, " ", pos);
    string halfmove_clock_s = next_token(fen, " ", pos);
    string fullmove_number_s = next_token(fen, " ", pos);

    parse_fen_board(fen_board);
    side = active_color == "w" ? chess::White : chess::Black;
    for (const auto& c: available_castling) {
        if (c == '-') break;
        int color = toupper(c) == c ? chess::White : Black;
        int side = toupper(c) == 'K' ? King : Queen;
        castling[color][side] = true;
    }
    if (en_passant_square == "-") {
        en_passant = Empty;
    } else {
        int col = en_passant_square[0] - 'a';
        int row = en_passant_square[1] - '0';
        en_passant = row * 8 + col;
    }
    halfmove_clock = stoi(halfmove_clock_s);
    fullmove_number = stoi(fullmove_number_s);
    init_hash();
    update_bitboards();
}

void game_state::parse_fen_board(string& fen_board) {
    int row = 7;
    int col = 0;
    for (const auto& c: fen_board) {
        if (c >= '1' && c <= '8') {
            col += c - '0';
            continue;
        }
        if (c == '/') {
            Assert(col == 8)
            row--;
            col = 0;
            continue;
        }
        uint8_t index = row * 8 + col;
        switch (c) {
            case 'R': set_1(board[White][Rook], index); break;
            case 'N': set_1(board[White][Knight], index); break;
            case 'B': set_1(board[White][Bishop], index); break;
            case 'Q': set_1(board[White][Queen], index); break;
            case 'K': set_1(board[White][King], index); break;
            case 'P': set_1(board[White][Pawn], index); break;
            case 'r': set_1(board[Black][Rook], index); break;
            case 'n': set_1(board[Black][Knight], index); break;
            case 'b': set_1(board[Black][Bishop], index); break;
            case 'q': set_1(board[Black][Queen], index); break;
            case 'k': set_1(board[Black][King], index); break;
            case 'p': set_1(board[Black][Pawn], index); break;
            default: Assert(false)
        }
        col++;
    }
}

void game_state::init_hash() {
    for (uint8_t index = 0; index < 64; index++) {
        for (uint8_t color = Black; color <= White; color++) {
            for (uint8_t type = Queen; type <= Pawn; type++) {
                if (!get_bit(board[color][type], index)) continue;
                hash.invert_piece(index, color, type);
            }
        }
    }
    if (side == Black) hash.invert_move();
    for (uint8_t color = Black; color <= White; color++) {
        for (uint8_t type = Queen; type <= King; type++) {
            if (castling[color][type]) hash.invert_castling(color, type);
        }
    }
}

void game_state::update_bitboards() {
    side_board[White] = board[White][Queen] | board[White][King] | board[White][Rook] |
                        board[White][Knight] | board[White][Bishop] | board[White][Pawn];
    side_board[Black] = board[Black][Queen] | board[Black][King] | board[Black][Rook] |
                        board[Black][Knight] | board[Black][Bishop] | board[Black][Pawn];
    inv_side_board[White] = ~side_board[White];
    inv_side_board[Black] = ~side_board[Black];
    all = side_board[White] | side_board[Black];
    empty = ~all;
}

void game_state::apply_move(const chess_move& move) {
    Assert(side == move.side)
    remove_piece(move.from, move.side, move.attacker_type);
    add_piece(move.to, move.side, move.attacker_type);
    if (move.defender_type != Empty) {
        remove_piece(move.to, inverse_color(move.side), move.defender_type);
    }
    switch (move.flag) {
        case chess_move::move_flag::Default:
            break;
        case chess_move::move_flag::PawnLongMove:
            en_passant = (move.from + move.to) / 2;
            break;
        case chess_move::move_flag::EnPassantCapture:
            if (move.side == White) {
                remove_piece(move.to - 8, Black, Pawn);
            } else {
                remove_piece(move.to + 8, White, Pawn);
            }
            break;
        case chess_move::move_flag::WhiteLongCastling:
            remove_piece(0, White, Rook);
            add_piece(3, White, Rook);
            castling_happened[White] = true;
            break;
        case chess_move::move_flag::WhiteShortCastling:
            remove_piece(7, White, Rook);
            add_piece(5, White, Rook);
            castling_happened[White] = true;
            break;
        case chess_move::move_flag::BlackLongCastling:
            remove_piece(56, Black, Rook);
            add_piece(59, Black, Rook);
            castling_happened[Black] = true;
            break;
        case chess_move::move_flag::BlackShortCastling:
            remove_piece(63, Black, Rook);
            add_piece(61, Black, Rook);
            castling_happened[Black] = true;
            break;
        case chess_move::move_flag::PromoteToRook:
            remove_piece(move.to, move.side, Pawn);
            add_piece(move.to, move.side, Rook);
            break;
        case chess_move::move_flag::PromoteToQueen:
            remove_piece(move.to, move.side, Pawn);
            add_piece(move.to, move.side, Queen);
            break;
        case chess_move::move_flag::PromoteToKnight:
            remove_piece(move.to, move.side, Pawn);
            add_piece(move.to, move.side, Knight);
            break;
        case chess_move::move_flag::PromoteToBishop:
            remove_piece(move.to, move.side, Pawn);
            add_piece(move.to, move.side, Bishop);
            break;
        default: Assert(false)
    }
    update_bitboards();
    if (move.flag != chess_move::move_flag::PawnLongMove) {
        en_passant = chess::Empty;
    }
    switch (move.from) {
        case 4:
            break_castling(White, Queen);
            break_castling(White, King);
            break;
        case 60:
            break_castling(Black, Queen);
            break_castling(Black, King);
            break;
    }
    if (!get_bit(board[White][Rook], 0))  break_castling(White, Queen);
    if (!get_bit(board[White][Rook], 7))  break_castling(White, King);
    if (!get_bit(board[Black][Rook], 56)) break_castling(Black, Queen);
    if (!get_bit(board[Black][Rook], 63)) break_castling(Black, King);
    if (move.side == Black) {
        fullmove_number++;
    }
    // en passant capture is handled here, because attacker figure is pawn here
    if (move.attacker_type == Pawn || move.defender_type != Empty) {
        halfmove_clock = 0;
    } else {
        halfmove_clock++;
    }
    invert_side();
}

bool game_state::is_check() const {
    uint8_t king_index = lsb(board[side][chess::King]); 
    return chess_move_generator::in_danger(*this, king_index, side);
}

void game_state::add_piece(uint8_t index, uint8_t color, uint8_t type) {
    Assert(!get_bit(board[color][type], index))
    set_1(board[color][type], index);
    hash.invert_piece(index, color, type);
}

void game_state::remove_piece(uint8_t index, uint8_t color, uint8_t type) {
    Assert(get_bit(board[color][type], index))
    set_0(board[color][type], index);
    hash.invert_piece(index, color, type);
}

void game_state::break_castling(uint8_t color, uint8_t type) {
    if (castling[color][type]) {
        castling[color][type] = false;
        hash.invert_castling(color, type);
    }
}

void game_state::invert_side() {
    side = inverse_color(side);
    hash.invert_move();
}

pair<uint8_t, uint8_t> get_piece(const game_state& state, int col, int row) {
    int index = row * 8 + col;
    for (uint8_t side = chess::Black; side <= chess::White; side++) {
        for (uint8_t piece = chess::Queen; piece <= chess::Pawn; piece++) {
            if (get_bit(state.board[side][piece], index)) {
                return make_pair(piece, side);
            }
        }
    }
    return make_pair(chess::Empty, chess::Black);
}

char piece_to_char(uint8_t side, uint8_t piece) {
    if (side) {
        switch (piece) {
            case chess::Queen: return 'Q';
            case chess::King: return 'K';
            case chess::Knight: return 'N';
            case chess::Bishop: return 'B';
            case chess::Rook: return 'R';
            case chess::Pawn: return 'P';
            default: Assert(false);
        }
    } else {
        switch (piece) {
            case chess::Queen: return 'q';
            case chess::King: return 'k';
            case chess::Knight: return 'n';
            case chess::Bishop: return 'b';
            case chess::Rook: return 'r';
            case chess::Pawn: return 'p';
            default: Assert(false);
        }
    }
}

std::string game_state::fen() const {
    stringstream ss;
    for (int row = 7; row >= 0; row--) {
        int empty_chain = 0;
        for (int col = 0; col < 9; col++) {
            auto [piece, piece_side] = get_piece(*this, col, row);
            if ((col == 8 || piece != chess::Empty) && empty_chain > 0) {
                ss << char(empty_chain + '0');
                empty_chain = 0;
            }
            if (col == 8) break;
            if (piece == chess::Empty) {
                empty_chain++;
                continue;
            }
            ss << piece_to_char(piece_side, piece);
        }
        if (row > 0) {
            ss << '/';
        }
    }
    ss << ' ' << (side == chess::White ? 'w' : 'b') << ' ';
    bool any_castling = castling[0][0] || castling[0][1] || castling[1][0] || castling[1][1];
    if (!any_castling) {
        ss << '-';
    } else {
        if (castling[chess::White][chess::King]) ss << 'K';
        if (castling[chess::White][chess::Queen]) ss << 'Q';
        if (castling[chess::Black][chess::King]) ss << 'k';
        if (castling[chess::Black][chess::Queen]) ss << 'q';
    }
    ss << ' ';
    if (en_passant == chess::Empty) {
        ss << '-';
    } else {
        ss << char('a' + en_passant % 8);
        ss << char('1' + en_passant / 8);
    }
    ss << ' ' << halfmove_clock << ' ' << fullmove_number;
    return ss.str();
}
