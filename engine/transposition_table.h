#ifndef CHESSUCIENGINE_TRANSPOSITION_TABLE_H
#define CHESSUCIENGINE_TRANSPOSITION_TABLE_H

#include "game_state.h"
#include <unordered_map>

class transposition_table {
    struct entry {
        int depth;
        chess_move best_move;
        bool is_pv;
    };
    
    std::unordered_map<uint64_t, entry> map;
public:
    void add(const game_state& state, int depth, chess_move best_move, bool is_pv);
    chess_move try_get_best_move(const game_state& state);
};


#endif //CHESSUCIENGINE_TRANSPOSITION_TABLE_H
