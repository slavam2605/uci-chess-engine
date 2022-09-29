#ifndef CHESSUCIENGINE_CHESS_UTILS_H
#define CHESSUCIENGINE_CHESS_UTILS_H

#include <cstdint>
#include <string>

namespace chess {
    static constexpr uint8_t Black = 0;
    static constexpr uint8_t White = 1;
    static constexpr uint8_t Queen = 0;
    static constexpr uint8_t King = 1;
    static constexpr uint8_t Rook = 2;
    static constexpr uint8_t Knight = 3;
    static constexpr uint8_t Bishop = 4;
    static constexpr uint8_t Pawn = 5;
    static constexpr uint8_t Empty = 255;
    static constexpr uint8_t EmptyPiece = 7;
    static constexpr uint8_t MinPiece = Queen;
    static constexpr uint8_t MaxPiece = Pawn;

    inline uint8_t inverse_color(uint8_t side) {
        return side == White ? Black : White;
    }
    
    inline std::string figure_to_string(uint8_t figure_type) {
        switch (figure_type) {
            case Queen: return "Queen";
            case King: return "King";
            case Rook: return "Rook";
            case Knight: return "Knight";
            case Bishop: return "Bishop";
            case Pawn: return "Pawn";
            default: return "<Unknown figure type>";
        }
    }
}

#endif //CHESSUCIENGINE_CHESS_UTILS_H
