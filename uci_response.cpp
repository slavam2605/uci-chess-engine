#include "uci_response.h"
#include "utils.h"

using namespace std;

string next_token(const string& s, int& pos) {
    string result;
    while (s[pos] == ' ') pos++;
    for (; pos < s.length(); pos++) {
        if (s[pos] == ' ') {
            pos++;
            break;
        }
        result.push_back(s[pos]);
    }
    return result;
}

uci_response uci_response::parse(const string& line) {
    uci_response result;
    int pos = 0;
    result.tag = next_token(line, pos);
    if (result.tag == TAG_ID) {
        string subtag = next_token(line, pos);
        Assert(subtag == ID_NAME || subtag == ID_AUTHOR)
        result.value[subtag] = line.substr(pos);
        return result;
    }
    if (result.tag == TAG_BESTMOVE) {
        string move = next_token(line, pos);
        result.value[BESTMOVE_MOVE] = move;
        string ponder_kw = next_token(line, pos);
        if (ponder_kw == BESTMOVE_PONDER) {
            string ponder_move = next_token(line, pos);
            result.value[BESTMOVE_PONDER] = ponder_move;
        }
        return result;
    }
    result.value[UCI_TAIL] = line.substr(pos);
    return result;
}

bool uci_response::get_value(const string& key, string& out_value) {
    auto it = value.find(key);
    if (it == value.end()) return false;
    out_value = it->second;
    return true;
}
