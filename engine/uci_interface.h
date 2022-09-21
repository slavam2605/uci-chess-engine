#ifndef CHESSUCIENGINE_UCI_INTERFACE_H
#define CHESSUCIENGINE_UCI_INTERFACE_H

#include <string>

std::string find_best_move(const std::string& fen, int depth);

bool is_mate(const std::string& fen);

#endif //CHESSUCIENGINE_UCI_INTERFACE_H
