#include "transposition_table.h"

void transposition_table::add(const game_state& state, int depth, chess_move best_move, bool is_pv) {
    auto hash = state.hash.value;
    auto iter = map.find(hash);
    if (iter == map.end() || iter->second.depth < depth || (!iter->second.is_pv && is_pv)) {
        map[hash] = entry {depth, best_move, is_pv};
    }
}

chess_move transposition_table::try_get_best_move(const game_state& state) {
    auto hash = state.hash.value;
    auto iter = map.find(hash);
    if (iter == map.end()) return move::Invalid;
    return iter->second.best_move;
}