#ifndef CHESSUCIENGINE_KILLER_TABLE_H
#define CHESSUCIENGINE_KILLER_TABLE_H

#include <vector>
#include <unordered_set>
#include "chess_move.h"

class killer_table {
    std::vector<std::unordered_set<chess_move>> killers;
public:
    static const killer_table Empty;
    
    killer_table();
    void add_killer(int depth, const chess_move& move);
    void clear(int depth);
    [[nodiscard]] bool is_killer(int depth, const chess_move& move) const;
};


#endif //CHESSUCIENGINE_KILLER_TABLE_H
