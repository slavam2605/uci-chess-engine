#include "game_state.h"
#include "dynamic_evaluator.h"
#include "magic/magic_generator.h"
#include "debug_tools.h"
#include <iostream>

using namespace std;

int main() {
    debug_tools::print_values_for_moves("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
//    debug_tools::print_values_for_moves("r1bqr1k1/ppp1ppbp/3p4/3N2B1/3PP1nQ/5N2/PPP2PP1/R3KB2 b Q - 8 11");
    return 0;
}