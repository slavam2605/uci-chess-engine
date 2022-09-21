#include "gui_chess_move.h"
#include "chess_utils.h"
#include <iostream>

using namespace std;

#define AssertMove(block, s) if (!(block)) {\
    cerr << "Unknown move format: '" << (s) << "'" << endl;\
    exit(1);\
}

chess_cell::chess_cell(int col, int row) : col(col), row(row) {}

chess_cell::chess_cell() : col(-1), row(-1) {}

bool operator==(const chess_cell& lhs, const chess_cell& rhs) {
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

std::string chess_cell::to_string() const {
    return string() + char(col + 'a') + char(row + '1');
}

gui_chess_move gui_chess_move::parse(const string& input) {
    gui_chess_move result;
    string s = input;
    while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\0') {
        s.pop_back();
    }
    AssertMove(s.length() >= 4, s)
    AssertMove(s.length() <= 5, s)
    int from_col = s[0] - 'a';
    int from_row = s[1] - '1';
    int to_col = s[2] - 'a';
    int to_row = s[3] - '1';
    AssertMove(from_col >= 0 && from_col < 8, s)
    AssertMove(from_row >= 0 && from_col < 8, s)
    AssertMove(to_col >= 0 && from_col < 8, s)
    AssertMove(to_row >= 0 && from_col < 8, s)
    result.from = chess_cell(from_col, from_row);
    result.to = chess_cell(to_col, to_row);
    if (s.length() == 5) {
        auto promotion = s[4];
        AssertMove(chess::allowed_promotion(promotion), s)
        result.promotion = promotion;
    }
    return result;
}

ostream& operator<<(ostream& stream, const gui_chess_move& move) {
    stream << move.from.to_string() << move.to.to_string();
    if (move.promotion != '\0') stream << move.promotion;
    return stream;
}

bool operator==(const gui_chess_move& lhs, const gui_chess_move& rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to && lhs.promotion == rhs.promotion;
}
