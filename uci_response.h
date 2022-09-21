#ifndef CHESSUCIENGINE_UCI_RESPONSE_H
#define CHESSUCIENGINE_UCI_RESPONSE_H

#include <unordered_map>

constexpr const char* TAG_ID = "id";
constexpr const char* TAG_UCIOK = "uciok";
constexpr const char* TAG_READYOK = "readyok";
constexpr const char* TAG_BESTMOVE = "bestmove";
constexpr const char* ID_NAME = "name";
constexpr const char* ID_AUTHOR = "author";
constexpr const char* BESTMOVE_MOVE = "move";
constexpr const char* BESTMOVE_PONDER = "ponder";
constexpr const char* UCI_TAIL = "uci_tail";

struct uci_response {
    std::string tag;
    std::unordered_map<std::string, std::string> value;
    
    bool get_value(const std::string& key, std::string& out_value);
    
    static uci_response parse(const std::string& line);
};


#endif //CHESSUCIENGINE_UCI_RESPONSE_H
