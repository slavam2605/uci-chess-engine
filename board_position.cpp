#include "board_position.h"
#include "utils.h"
#include "chess_utils.h"
#include "engine/uci_interface.h"
#include <sstream>
#include <iostream>

using namespace std;

board_position::board_position()
        : board_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {}

board_position::board_position(const string& fen)
        : is_white(true), halfmove_clock(0), fullmove_number(0),
          castling{{true, true},
                   {true, true}} {
    memset(board, ' ', sizeof(board));
    int row = 7;
    int col = 0;
    int i = 0;
    for (; i < fen.length(); i++) {
        if (row == 0 && col == 8) break;
        char c = fen[i];
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
        board[col][row] = c;
        col++;
    }
    if (i < fen.length() - 1) {
        i++;
        if (fen[i] == 'b') is_white = false;
    }
}

bool board_position::is_white_turn() const {
    return is_white;
}

string board_position::get_fen() const {
    stringstream s;
    for (int row = 7; row >= 0; row--) {
        int empty_chain = 0;
        for (int col = 0; col < 9; col++) {
            if ((col == 8 || board[col][row] != ' ') && empty_chain > 0) {
                s << char(empty_chain + '0');
                empty_chain = 0;
            }
            if (col == 8) break;
            if (board[col][row] == ' ') {
                empty_chain++;
                continue;
            }
            s << board[col][row];
        }
        if (row > 0) {
            s << '/';
        }
    }
    s << ' ' << (is_white ? 'w' : 'b') << ' ';
    bool any_castling = castling[0][0] || castling[0][1] || castling[1][0] || castling[1][1];
    if (!any_castling) {
        s << '-';
    } else {
        if (castling[WHITE_CASTLING][SHORT_CASTLING]) s << 'K';
        if (castling[WHITE_CASTLING][LONG_CASTLING]) s << 'Q';
        if (castling[BLACK_CASTLING][SHORT_CASTLING]) s << 'k';
        if (castling[BLACK_CASTLING][LONG_CASTLING]) s << 'q';
    }
    s << ' ';
    if (en_passant == chess_cell()) {
        s << '-';
    } else {
        s << en_passant.to_string();
    }
    s << ' ' << halfmove_clock << ' ' << fullmove_number;
    return s.str();
}

const vector<gui_chess_move>& board_position::get_moves() const {
    return moves;
}

void board_position::add_move(const gui_chess_move& move) {
    Assert(apply_move(move))
    moves.push_back(move);
}

bool board_position::apply_move(const gui_chess_move& move) {
    char fig = board[move.from.col][move.from.row];
    char& from = board[move.from.col][move.from.row];
    char& to = board[move.to.col][move.to.row];
    Assert(fig != ' ')
    Assert(move.promotion == '\0' || chess::is_pawn(fig))
    Assert(chess::is_white(fig) == is_white)
    bool capture = false;
    bool long_pawn_move = false;
    if (chess::is_rook(fig)) {
        
    } else if (chess::is_knight(fig)) {
        
    } else if (chess::is_bishop(fig)) {
        
    } else if (chess::is_queen(fig)) {
        
    } else if (chess::is_king(fig)) {
        if (try_castling(move)) goto apply_end;
        auto col_diff = abs(move.from.col - move.to.col);
        auto row_diff = abs(move.from.row - move.to.row);
        if (col_diff > 1 || row_diff > 1 || col_diff + row_diff == 0) return false;
    } else if (chess::is_pawn(fig)) {
        // TODO support en passant
        auto col_diff = abs(move.from.col - move.to.col);
        auto row_diff = abs(move.from.row - move.to.row);
        Assert(col_diff <= 1 && row_diff <= 2)
        Assert(row_diff > 0)
        Assert(is_white == (move.to.row > move.from.row))
        if (col_diff == 0) { // capture
            if (row_diff == 2) {
                Assert(move.from.row == 1 || move.from.row == 6)
                en_passant = chess_cell(move.from.col, (move.from.row + move.to.row) / 2);
                long_pawn_move = true;
            }
            Assert(to == ' ')
        } else { // capture
            Assert(row_diff == 1)
            Assert(to != ' ' || move.to == en_passant)
            capture = true;
            if (move.to == en_passant) {
                int captured_col = en_passant.col;
                int captured_row = en_passant.row + (is_white ? -1 : +1);
                Assert(board[captured_col][captured_row] == (is_white ? 'p' : 'P'))
                board[captured_col][captured_row] = ' ';
            }
        }
        if (move.promotion != '\0') {
            Assert(move.to.row == 0 || move.to.row == 7)
        }
    } else {
        Assert(false)
    }
    if (to != ' ') { capture = true; }
    to = move.promotion == '\0' ? from : chess::to_color(move.promotion, is_white);
    from = ' ';
apply_end:
    if (board[0][0] != 'R') { castling[WHITE_CASTLING][LONG_CASTLING] = false; }
    if (board[7][0] != 'R') { castling[WHITE_CASTLING][SHORT_CASTLING] = false; }
    if (board[4][0] != 'K') {
        castling[WHITE_CASTLING][LONG_CASTLING] = false;
        castling[WHITE_CASTLING][SHORT_CASTLING] = false;
    }
    if (board[0][7] != 'r') { castling[BLACK_CASTLING][LONG_CASTLING] = false; }
    if (board[7][7] != 'r') { castling[BLACK_CASTLING][SHORT_CASTLING] = false; }
    if (board[4][7] != 'k') {
        castling[BLACK_CASTLING][LONG_CASTLING] = false;
        castling[BLACK_CASTLING][SHORT_CASTLING] = false;
    }
    if (!long_pawn_move) {
        en_passant = chess_cell();
    }
    if (!is_white) {
        fullmove_number++;
    }
    if (capture || chess::is_pawn(fig)) {
        halfmove_clock = 0;
    } else {
        halfmove_clock++;
    }
    is_white = !is_white;
    return true;
}

bool board_position::try_castling(const gui_chess_move& move) {
    auto from_fig = board[move.from.col][move.from.row];
    if (move.from.col != 4) return false;
    if (move.from.row != 0 && move.from.row != 7) return false;
    if (move.to.col != 2 && move.to.col != 6) return false;
    if (move.to.row != move.from.row) return false;
    auto is_long = move.to.col == 2;
    auto row = move.from.row;
    auto rook_col = is_long ? 0 : 7;
    auto rook = from_fig == 'K' ? 'R' : 'r';
    Assert(board[rook_col][row] == rook)
    for (int i = min(move.from.col, rook_col) + 1; i < max(move.from.col, rook_col); i++) {
        Assert(board[i][row] == ' ')
    }
    auto new_rook_col = is_long ? 3 : 5;
    board[new_rook_col][row] = rook;
    board[move.to.col][move.to.row] = from_fig;
    board[move.from.col][move.from.row] = ' ';
    board[rook_col][row] = ' ';
    return true;
}

match_result board_position::get_match_result() const {
    if (halfmove_clock >= 100) return match_result::DRAW;
    if (is_mate(get_fen())) {
        return is_white ? match_result::BLACK_WIN : match_result::WHITE_WIN;
    } 
    return match_result::NONE;
}

#define UnicodeSymbols true
#if UnicodeSymbols
#define pick(a, b) b
#else
#define pick(a, b) a
#endif

ostream& operator<<(ostream& stream, const board_position& board) {
    auto ltc = pick(char(218), "\u250C");
    auto rtc = pick(char(191), "\u2510");
    auto lbc = pick(char(192), "\u2514");
    auto rbc = pick(char(217), "\u2518");
    auto tt = pick(char(194), "\u252C");
    auto bt = pick(char(193), "\u2534");
    auto lt = pick(char(195), "\u251C");
    auto rt = pick(char(180), "\u2524");
    auto cross = pick(char(197), "\u253C");
    auto h = pick(char(196), "\u2500");
    auto v = pick(char(179), "\u2502");
    
    stream << ltc;
    for (int i = 0; i < 7; i++) {
        stream <<  h << h << h << tt;
    }
    stream << h << h << h << rtc << endl;
    
    for (int row = 7; row >= 0; row--) {
        stream << v;
        for (int col = 0; col < 8; col++) {
            stream << ' ' << board.board[col][row] << ' ' << v;
        }
        stream << endl;

        if (row == 0) continue;
        stream << lt;
        for (int i = 0; i < 7; i++) {
            stream <<  h << h << h << cross;
        }
        stream <<  h << h << h << rt << endl;
    }

    stream << lbc;
    for (int i = 0; i < 7; i++) {
        stream <<  h << h << h << bt;
    }
    stream <<  h << h << h << rbc << endl;
    
    return stream;
}
