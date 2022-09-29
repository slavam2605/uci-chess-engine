#include "uci_interface.h"
#include "game_state.h"
#include "dynamic_evaluator.h"
#include "chess_move_generator.h"
#include <sstream>
#include <chrono>

using namespace std;

string find_best_move(const string& fen, int depth) {
    game_state state(fen);
    auto start = chrono::steady_clock::now();
    dynamic_evaluator evaluator;
    auto move = evaluator.find_best_move(state, depth);
    cout << "find_best_move: " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() << " ms" << endl;
    stringstream ss;
    ss << move::to_string(move);
    return ss.str();
}

bool is_mate(const string& fen) {
    game_state state(fen);
    move_list moves;
    chess_move_generator::generate_all_moves(moves, state, state.side);
    return moves.size() == 0;
}