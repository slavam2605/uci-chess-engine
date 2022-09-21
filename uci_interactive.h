#ifndef CHESSUCIENGINE_UCI_INTERACTIVE_H
#define CHESSUCIENGINE_UCI_INTERACTIVE_H

#include <string>
#include <thread>
#include <functional>
#include <chrono>
#include "process_interaction.h"
#include "uci_response.h"
#include "gui_chess_move.h"
#include "board_position.h"
#include "blocking_queue.h"

struct engine_data {
    bool uciok;
    std::string name, author;
};

class uci_interactive {
    process_interaction process;
    blocking_queue<uci_response> message_queue;
    std::thread worker_thread;

public:
    uci_interactive(const std::string& exe_path, const std::function<void(uci_interactive&)>& worker);
    engine_data begin_uci();
    bool wait_ready();
    gui_chess_move go_bestmove(const std::chrono::milliseconds& time_for_move, int depth = -1);
    void send_position(const board_position& board);
};


#endif //CHESSUCIENGINE_UCI_INTERACTIVE_H
