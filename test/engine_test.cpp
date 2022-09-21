#include "../engine/chess_move_generator.h"
#include "../engine/dynamic_evaluator.h"
#include "../engine/static_evaluator.h"
#include "perft_utils.h"
#include <chrono>
#include <utility>

using namespace std;

struct test_case {
    string fen;
    vector<uint64_t> nodes;

    test_case(string fen, const vector<uint64_t>& nodes) : fen(std::move(fen)), nodes(nodes) {}
};

int get_nodes_count(const game_state& state, int depth, uint8_t side) { // NOLINT(misc-no-recursion)
    if (depth == 0) return 1;
    move_list moves;
    chess_move_generator::generate_all_moves(moves, state, side);
    int total = 0;
    for (const auto& move: moves) {
        game_state new_state(state);
        new_state.apply_move(move);
        auto child_total = get_nodes_count(new_state, depth - 1, chess::inverse_color(side));
        total += child_total;
    }
    return total;
}

void run_performance_work() {
    game_state state("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1");
    get_nodes_count(state, 4, state.side);
}

void performance_test(void work(), int BatchSize = 1, int WarmupIterations = 3, int MeasureIterations = 10) {
    for (int i = 0; i < WarmupIterations; i++) {
        auto start = chrono::steady_clock::now();
        for (int _ = 0; _ < BatchSize; _++) {
            work();
        }
        auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
        cout << "Warmup batch time: " << duration.count() / BatchSize << " ms" << endl;
    }
    
    auto global_start = chrono::steady_clock::now();
    for (int i = 0; i < MeasureIterations; i++) {
        auto start = chrono::steady_clock::now();
        for (int _ = 0; _ < BatchSize; _++) {
            work();
        }
        auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
        cout << "Measure batch time: " << duration.count() / BatchSize << " ms" << endl;
    }
    auto global_duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - global_start);
    auto average = global_duration.count() / (double) (BatchSize * MeasureIterations);
    cout << "Average time: " << (int) average << " ms" << endl;
}

void run_tests(const vector<test_case>& cases) {
    for (const auto& test_case: cases) {
        cout << "Test case: " << test_case.fen << endl;
        game_state state(test_case.fen);
        for (int depth = 0; depth < test_case.nodes.size(); depth++) {
            int actual = get_nodes_count(state, depth, state.side);
            if (test_case.nodes[depth] != actual) {
                cerr << "\tDepth: " << depth << endl;
                cerr << "\tExpected: " << test_case.nodes[depth] << endl;
                cerr << "\tActual: " << actual << endl;
                exit(1);
            } else {
                cout << "\tDepth: " << depth << ", ok" << endl;
            }
        }
    }
}

void test_starting_value_zero() {
    game_state state("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
    Assert(static_evaluator::evaluate(state) == 0)
}

void perft_test(int depth) {
    auto result = perft_utils::perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1", depth);
    size_t total = 0;
    for (const auto& [move, nodes]: result) {
//        cout << move << ": " << nodes << endl;
        total += nodes;
    }
//    cout << "Nodes searched: " << total << endl;
}

int main() {
//    vector<test_case> test_cases = {
//            {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", {1, 20, 400, 8902, 197281, 4865609}},
//            {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1", {1, 44, 1486, 62379, 2103487, 89941194}},
//            {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", {1, 48, 2039, 97862, 4085603, 193690690}},
//            {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", {1, 14, 191, 2812, 43238, 674624}},
//            {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", {1, 6, 264, 9467, 422333, 15833292}},
//            {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", {1, 46, 2079, 89890, 3894594, 164075551}},
//    };
//    run_tests(test_cases);
//    return 0;

//    performance_test(run_performance_work);
    performance_test([](){ perft_test(5); });
//    performance_test([](){ perft_test(6); }, 1, 0, 1);
    return 0;

    game_state state("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 0 1");
    dynamic_evaluator evaluator;
    auto start = chrono::steady_clock::now();
    evaluator.find_best_move(state, 9);
    auto diff = chrono::steady_clock::now() - start;
    cout << chrono::duration_cast<chrono::milliseconds>(diff).count() << " ms" << endl;
    cout << "Max depth: " << evaluator.max_depth << endl;
    cout << "Total nodes in main search: " << evaluator.main_search_nodes << endl;
    cout << "Total nodes in zero window search: " << evaluator.zero_window_nodes << endl;
    cout << "Total nodes in capture search: " << evaluator.capture_search_nodes << endl;
    cout << "Found entry in transposition table: " << evaluator.transposition_found << endl;
    cout << "Move from transposition table was best: " << evaluator.transposition_best_hit << endl;
    cout << "PVS research count: " << evaluator.pvs_research_count << endl;

    return 0;
}