#include "uci_interactive.h"
#include "utils.h"
#include <sstream>

using namespace std;

uci_interactive::uci_interactive(const string& exe_path, const function<void(uci_interactive&)>& worker)
        : process(exe_path, [this](const string& line) {
            auto response = uci_response::parse(line);
            message_queue.push(response); 
        }), worker_thread(worker, ref(*this)) {
    process.start_reading();
}

engine_data uci_interactive::begin_uci() {
    engine_data result;
    process.write("uci\n");
    // TODO timeout -> false
    while (true) {
        auto response = message_queue.pop();
        if (response.tag == TAG_ID) {
            response.get_value(ID_NAME, result.name);
            response.get_value(ID_AUTHOR, result.author);
        }
        if (response.tag == TAG_UCIOK) {
            result.uciok = true;
            return result;
        }
    }
    result.uciok = false;
    return result;
}

bool uci_interactive::wait_ready() {
    process.write("isready\n");
    // TODO timeout -> false
    while (true) {
        auto response = message_queue.pop();
        if (response.tag == TAG_READYOK) {
            return true;
        }
    }
    return false;
}

gui_chess_move uci_interactive::go_bestmove(const chrono::milliseconds& time_for_move, int depth) {
    if (depth < 0) {
        process.write("go movetime " + to_string(time_for_move.count()) + "\n");
    } else {
        process.write("go depth " + to_string(depth) + "\n");
    }
    // TODO timeout -> some error
    while (true) {
        auto response = message_queue.pop();
        if (response.tag == TAG_BESTMOVE) {
            string move;
            Assert(response.get_value(BESTMOVE_MOVE, move))
            return gui_chess_move::parse(move);
        }
    }
}

void uci_interactive::send_position(const board_position& board) {
    stringstream ss;
    ss << "position fen " << board.get_fen() << "\n";
    process.write(ss.str());
}
