#include "perft_utils.h"

using namespace std;

std::vector<pair<chess_move, size_t>> perft_utils::perft(const string& fen, int depth) {
    vector<pair<chess_move, size_t>> result;
    game_state state(fen);
    move_list_pool pool;
    move_list& moves = pool.init_list(depth);
    chess_move_generator::generate_all_moves(moves, state, state.side);
    for (const auto& move: moves) {
        game_state new_state(state);
        new_state.apply_move(move);
        auto nodes = perft_inner(new_state, pool, depth - 1);
        result.emplace_back(move, nodes);
    }
    return result;
}

size_t perft_utils::perft_inner(const game_state& state, move_list_pool& pool, int depth) { // NOLINT(misc-no-recursion)
    if (depth == 0) return 1;
    size_t result = 0;
    move_list& moves = pool.init_list(depth);
    chess_move_generator::generate_all_moves(moves, state, state.side);
    if (depth == 1) return moves.size();
    for (const auto& move: moves) {
        game_state new_state(state);
        new_state.apply_move(move);
        if (depth > 2) {
            result += perft_inner(new_state, pool, depth - 1);
        } else {
            move_list& new_moves = pool.init_list(depth - 1);
            chess_move_generator::generate_all_moves(new_moves, new_state, new_state.side);
            result += new_moves.size();
        }
    }
    return result;
}
