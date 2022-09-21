#ifndef CHESSUCIENGINE_PERFT_UTILS_H
#define CHESSUCIENGINE_PERFT_UTILS_H

#include "../engine/chess_move.h"
#include "../engine/chess_move_generator.h"
#include <vector>

class perft_utils {
    static size_t perft_inner(const game_state& state, int depth);
public:
    static std::vector<std::pair<chess_move, size_t>> perft(const std::string& fen, int depth);
};


#endif //CHESSUCIENGINE_PERFT_UTILS_H
