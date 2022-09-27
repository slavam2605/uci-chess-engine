#include "killer_table.h"

const killer_table killer_table::Empty;

killer_table::killer_table() : killers(250) {}

void killer_table::add_killer(int depth, const chess_move& move) {
    ensure_size(depth);
    killers[depth].insert(move);
}

void killer_table::clear(int depth) {
    ensure_size(depth);
    killers[depth].clear();
}

bool killer_table::is_killer(int depth, const chess_move& move) const {
    ensure_size(depth);
    return killers[depth].find(move) != killers[depth].end();
}

void killer_table::ensure_size(int depth) const {
    while (killers.size() <= depth) {
        killers.resize(killers.size() * 2);
    }
}
