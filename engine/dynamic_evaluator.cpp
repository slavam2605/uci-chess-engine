#include "dynamic_evaluator.h"
#include "chess_move_generator.h"
#include "static_evaluator.h"
#include <limits>
#include <algorithm>

#define CollectStatistics false
#if CollectStatistics
#define Stat(block) block
#else
#define Stat(block)
#endif

using namespace std;

dynamic_evaluator::dynamic_evaluator() :
        pool(), max_depth(0), main_search_nodes(0), zero_window_nodes(0), capture_search_nodes(0), 
        transposition_found(0), transposition_best_hit(0), pvs_research_count(0) {}

int32_t dynamic_evaluator::eval_move(const chess_move& move, const game_state& state, bitboard pawn_capture_mask, bool is_killer) {
    int32_t result = 0;
    if (move::attacker(move) != chess::Pawn && get_bit(pawn_capture_mask, move::to(move))) {
        result -= static_evaluator::material_cost[move::attacker(move)];
    }
    if (move::defender(move) != chess::EmptyPiece) {
        result += 1000 * static_evaluator::material_cost[move::defender(move)];
        result -= static_evaluator::material_cost[move::attacker(move)];
    }
    if (is_killer) { // place killers right after all captures
        result += 10000;
    }
    return result;
}

void dynamic_evaluator::sort_moves(move_list& moves, const game_state& state, uint8_t side, 
                                   const chess_move& hash_move, const killer_table& ktable, int real_depth) {
    bitboard pawn_capture_mask =
            legal_move_mask::generate_left_pawn_capture_mask(state, chess::inverse_color(side), true) |
            legal_move_mask::generate_right_pawn_capture_mask(state, chess::inverse_color(side), true);
    for (auto &move : moves) {
        move.evaluation = eval_move(move, state, pawn_capture_mask, ktable.is_killer(real_depth, move));
    }
    sort(moves.begin(), moves.end(), [&hash_move](const auto& a, const auto& b) {
        if (a == hash_move) return true;
        if (b == hash_move) return false;
        return a.evaluation > b.evaluation;
    });
}

chess_move dynamic_evaluator::find_best_move(const game_state& state, int depth) {
    transposition_table table;
    killer_table ktable;
    chess_move best_move = chess_move::Invalid;
    int color = state.side == chess::White ? 1 : -1;
    for (int dd = 1; dd <= depth; dd++) {
        pvs(state, table, ktable, dd, 1, -numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), color, &best_move);
    }
    Assert(best_move.is_valid())
    return best_move;
}

int32_t dynamic_evaluator::pvs(const game_state& state, transposition_table& table, killer_table& ktable, // NOLINT(misc-no-recursion)
                               int depth, int real_depth, int32_t alpha, int32_t beta, 
                               int color, chess_move* best_move) {
    Stat(max_depth = max(max_depth, real_depth);)
    Stat(main_search_nodes++;)
    if (depth == 0) return nega_max_captures(state, real_depth, alpha, beta, color);

    move_list& moves = pool.init_list(real_depth);
    chess_move_generator::generate_all_moves(moves, state, color > 0 ? chess::White : chess::Black, false);

    chess_move hash_move = table.try_get_best_move(state);
    sort_moves(moves, state, color == 1 ? chess::White : chess::Black, hash_move, ktable, real_depth);

    Stat(if (hash_move.is_valid()) {
        transposition_found++;
    })
    
    ktable.clear(real_depth + 1);
    int32_t best_score = -numeric_limits<int32_t>::max();
    int best_index = -1;
    bool search_pv = true;
    for (int i = 0; i < moves.size(); i++) {
        const auto& move = moves[i];
        game_state new_state(state);
        new_state.apply_move(move);
        int new_depth = new_state.is_check() ? depth : depth - 1;
        int32_t score;
        if (search_pv) {
            score = -pvs(new_state, table, ktable, new_depth, real_depth + 1, -beta, -alpha, -color);
        } else {
            score = -zero_window_search(new_state, table, ktable, new_depth, real_depth + 1, -alpha, -color);
            if (alpha < score) {
                Stat(pvs_research_count++;)
                score = -pvs(new_state, table, ktable, new_depth, real_depth + 1, -beta, -alpha, -color);
            }
        }
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
        if (score > alpha) {
            search_pv = false;
        }
        alpha = max(alpha, score);
        if (alpha >= beta) {
            ktable.add_killer(real_depth, move);
            break;
        }
    }

    if (best_index >= 0) {
        Stat(if (hash_move.is_valid() && moves[best_index] == hash_move) {
            transposition_best_hit++;
        })
        table.add(state, depth, moves[best_index], true);
        if (best_move != nullptr) {
            *best_move = chess_move(moves[best_index]);
        }
    }

    return best_score;
}

int32_t dynamic_evaluator::zero_window_search(const game_state& state, transposition_table& table, killer_table& ktable, // NOLINT(misc-no-recursion) 
                                              int depth, int real_depth, int32_t beta, int color) {
    Stat(zero_window_nodes++;)
    if (depth == 0) return nega_max_captures(state, real_depth, beta - 1, beta, color);

    move_list& moves = pool.init_list(real_depth);
    chess_move_generator::generate_all_moves(moves, state, color > 0 ? chess::White : chess::Black, false);

    chess_move hash_move = table.try_get_best_move(state);
    sort_moves(moves, state, color == 1 ? chess::White : chess::Black, hash_move, ktable, real_depth);
    
    ktable.clear(real_depth + 1);
    for (const auto& move : moves) {
        game_state new_state(state);
        new_state.apply_move(move);
        int new_depth = new_state.is_check() ? depth : depth - 1;
        int32_t score = -zero_window_search(new_state, table, ktable, new_depth, real_depth + 1, 1 - beta, -color);
        if (score >= beta) {
            table.add(state, depth, move, false);
            ktable.add_killer(real_depth, move);
            return beta;
        }
    }
    
    return beta - 1;
}

int32_t dynamic_evaluator::nega_max_captures(const game_state& state, int real_depth, int32_t alpha, int32_t beta, int color) { // NOLINT(misc-no-recursion)
    Stat(max_depth = max(max_depth, real_depth);)
    Stat(capture_search_nodes++;)
    int32_t evaluation = color * static_evaluator::evaluate(state);
    alpha = max(alpha, evaluation);
    if (alpha >= beta) return beta;
    
    move_list& moves = pool.init_list(real_depth);
    uint8_t side = color > 0 ? chess::White : chess::Black;
    chess_move_generator::generate_all_moves(moves, state, side, true);
    sort_moves(moves, state, side, chess_move::Invalid, killer_table::Empty, real_depth);

    for (const auto& move : moves) {
        Assert(state.is_capture(move))
        game_state new_state(state);
        new_state.apply_move(move);
        evaluation = -nega_max_captures(new_state, real_depth + 1, -beta, -alpha, -color);
        alpha = max(alpha, evaluation);
        if (alpha >= beta) return beta;
    }

    return alpha;
}