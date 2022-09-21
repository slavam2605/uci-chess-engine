#include "killer_table.h"

const killer_table killer_table::Empty;

killer_table::killer_table() : killers(250) {}

void killer_table::add_killer(int depth, const chess_move& move) {
    killers[depth].insert(move);
}

void killer_table::clear(int depth) {
    killers[depth].clear();
}

bool killer_table::is_killer(int depth, const chess_move& move) const {
    return killers[depth].find(move) != killers[depth].end();
}
