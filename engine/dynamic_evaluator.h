#ifndef CHESSUCIENGINE_DYNAMIC_EVALUATOR_H
#define CHESSUCIENGINE_DYNAMIC_EVALUATOR_H

#include "chess_move.h"
#include "game_state.h"
#include "transposition_table.h"
#include "move_list.h"
#include "move_list_pool.h"
#include "killer_table.h"
#include <vector>

class dynamic_evaluator {
    move_list_pool pool;
    
    int32_t pvs(const game_state& state, transposition_table& table, killer_table& ktable,
                int depth, int real_depth, int32_t alpha, int32_t beta, int color,
                chess_move* best_move = nullptr);
    int32_t zero_window_search(const game_state& state, transposition_table& table, killer_table& ktable, 
                               int depth, int real_depth, int32_t beta, int color);
    int32_t nega_max_captures(const game_state& state,  
                              int real_depth, int32_t alpha, int32_t beta, int color);
    static void sort_moves(move_list& moves, const game_state& state, uint8_t side, 
                           const chess_move& hash_move, const killer_table& ktable, int real_depth);
    static int32_t eval_move(const chess_move& move, const game_state& state, bitboard pawn_capture_mask, bool is_killer);
public:
    int32_t max_depth;
    int32_t main_search_nodes;
    int32_t zero_window_nodes;
    int32_t capture_search_nodes;
    int32_t transposition_found;
    int32_t transposition_best_hit;
    int32_t pvs_research_count;
    chess_move find_best_move(const game_state& state, int depth);
    
    dynamic_evaluator();
    
    friend class debug_tools;
};


#endif //CHESSUCIENGINE_DYNAMIC_EVALUATOR_H
