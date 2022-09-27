#include "debug_tools.h"
#include "game_state.h"
#include "chess_move_generator.h"
#include "static_evaluator.h"
#include "dynamic_evaluator.h"
#include <iostream>

using namespace std;

void debug_tools::print_values_for_moves(const string& fen) {
    game_state state(fen);
    move_list moves;
    chess_move_generator::generate_all_moves(moves, state, state.side);
    transposition_table table;
    killer_table ktable;
    dynamic_evaluator evaluator;
    cout << "initial value: " << static_evaluator::evaluate(state) << " cp" << endl;
    for (const auto& move : moves) {
        game_state new_state(state);
        new_state.apply_move(move);
        auto value = evaluator.pvs(new_state, table, ktable, 6, 1, -1000000000, 1000000000,
                                   new_state.side == chess::White ? 1 : -1);
        cout << move << " => " << /*static_evaluator::evaluate(new_state)*/value << " cp" << endl;
    }
}
