#ifndef CHESSUCIENGINE_GUI_CHESS_MOVE_H
#define CHESSUCIENGINE_GUI_CHESS_MOVE_H

#include <string>

/**
 * col in [a-h] (0-7), row in [0-7]
 */
struct chess_cell {
    int col, row;

    chess_cell();
    chess_cell(int col, int row);
    [[nodiscard]] std::string to_string() const;
    chess_cell& operator=(const chess_cell& cell) = default;
    
    friend bool operator==(const chess_cell& lhs, const chess_cell& rhs);
};

struct gui_chess_move {
    chess_cell from, to;
    char promotion = '\0';
    
    static gui_chess_move parse(const std::string& s);
    gui_chess_move& operator=(const gui_chess_move& move) = default;
    friend std::ostream& operator<<(std::ostream& stream, const gui_chess_move& move);
    friend bool operator==(const gui_chess_move& lhs, const gui_chess_move& rhs);
};


#endif //CHESSUCIENGINE_GUI_CHESS_MOVE_H
