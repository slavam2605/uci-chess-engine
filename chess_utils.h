#ifndef CHESSUCIENGINE_CHESS_UTILS_H
#define CHESSUCIENGINE_CHESS_UTILS_H

#include <cctype>

namespace chess {
    inline bool allowed_promotion(char fig) {
        fig = (char) tolower(fig);
        switch (fig) {
            case 'r':
            case 'n':
            case 'b':
            case 'q':
                return true;
            default:
                return false;
        }
    }
    
    inline bool is_white(char fig) {
        return toupper(fig) == fig;
    }
    
    inline bool is_black(char fig) {
        return tolower(fig) == fig;
    }
    
    inline char to_color(char fig, bool is_white) {
        if (is_white) return toupper(fig);
        return tolower(fig);
    }

    inline bool is_rook(char fig) {
        return tolower(fig) == 'r';
    }

    inline bool is_knight(char fig) {
        return tolower(fig) == 'n';
    }

    inline bool is_bishop(char fig) {
        return tolower(fig) == 'b';
    }

    inline bool is_queen(char fig) {
        return tolower(fig) == 'q';
    }

    inline bool is_king(char fig) {
        return tolower(fig) == 'k';
    }

    inline bool is_pawn(char fig) {
        return tolower(fig) == 'p';
    }
}

#endif //CHESSUCIENGINE_CHESS_UTILS_H
