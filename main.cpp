#include <iostream>
#include "uci_interactive.h"
#include "board_position.h"
#include "utils.h"
#include "engine/uci_interface.h"
#include "engine/magic/magic_generator.h"

using namespace std;

constexpr const char* stockfish_path = R"(C:\Users\Vyacheslav.Moklev\Downloads\stockfish_15_win_x64_avx2\stockfish_15_win_x64_avx2\stockfish_15_x64_avx2.exe)";
//constexpr const char* stockfish_path = R"(C:\Users\Vyacheslav.Moklev\Downloads\Chenglite_v1.0_x64_windows.exe)";

int main() {
    uci_interactive uci(stockfish_path, [](uci_interactive& uci) {
        auto uci_support = uci.begin_uci();
        if (!uci_support.uciok) {
            cerr << "Current engine doesn't support UCI protocol, exit" << endl;
            exit(1);
        } else {
            cout << "Engine name: " << uci_support.name << endl;
            cout << "Engine author: " << uci_support.author << endl;
        }
        Assert(uci.wait_ready())

        board_position board;
        cout << board;
        uci.send_position(board);

        bool is_white = board.is_white_turn();
        bool match_ended = false;
        while (!match_ended) {
            gui_chess_move best_move;
            if (is_white) {
                best_move = uci.go_bestmove(chrono::milliseconds(100), 7);
//                best_move = gui_chess_move::parse(find_best_move(board.get_fen(), 8));
            } else {
                best_move = gui_chess_move::parse(find_best_move(board.get_fen(), 7));
            }
            cout << "Best move: " << best_move << endl;
            board.add_move(best_move);
            cout << "FEN: " << board.get_fen() << endl;
            uci.send_position(board);
            cout << board;
            is_white = !is_white;

            switch (board.get_match_result()) {
                case match_result::NONE: break;
                case match_result::WHITE_WIN:
                    cout << "White wins!" << endl;
                    match_ended = true;
                    break;
                case match_result::BLACK_WIN:
                    cout << "Black wins!" << endl;
                    match_ended = true;
                    break;
                case match_result::DRAW:
                    cout << "Draw!" << endl;
                    match_ended = true;
                    break;
            }
        }
    });

    return 0;
}
