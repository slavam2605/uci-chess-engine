#ifndef CHESSUCIENGINE_BOARD_POSITION_H
#define CHESSUCIENGINE_BOARD_POSITION_H

#include "gui_chess_move.h"
#include <vector>

enum class match_result {
    NONE, WHITE_WIN, BLACK_WIN, DRAW
};

class board_position {
    std::vector<gui_chess_move> moves;
    char board[8][8];
    bool is_white;
    int halfmove_clock, fullmove_number;
    bool castling[2][2]; // [black, white][short(king), long(queen)]
    chess_cell en_passant;
    
    
    bool try_castling(const gui_chess_move& move);
    bool apply_move(const gui_chess_move& move);
public:
    static constexpr int WHITE_CASTLING = 1;
    static constexpr int BLACK_CASTLING = 0;
    static constexpr int LONG_CASTLING = 1;
    static constexpr int SHORT_CASTLING = 0;
    
    board_position();
    explicit board_position(const std::string& fen);
    
    [[nodiscard]] bool is_white_turn() const;
    [[nodiscard]] std::string get_fen() const; 
    [[nodiscard]] const std::vector<gui_chess_move>& get_moves() const;
    [[nodiscard]] match_result get_match_result() const;
    void add_move(const gui_chess_move& move);

    friend std::ostream& operator<<(std::ostream& stream, const board_position& board);
};


#endif //CHESSUCIENGINE_BOARD_POSITION_H
