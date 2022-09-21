#ifndef CHESSUCIENGINE_GAME_STATE_H
#define CHESSUCIENGINE_GAME_STATE_H

#include <string>
#include <array>
#include "bitboard.h"
#include "chess_move.h"
#include "zobrist_hash.h"

struct game_state {
    std::array<std::array<bitboard, 6>, 2> board{}; // color, figure type
    std::array<bitboard, 2> side_board{};
    std::array<bitboard, 2> inv_side_board{};
    bitboard all = 0, empty = 0;
    bool castling[2][2]{}; // color, queen/king
    bool castling_happened[2]{}; // color
    uint8_t side;
    uint8_t en_passant;
    int halfmove_clock;
    int fullmove_number;
    zobrist_hash hash;

    void parse_fen_board(std::string& fen_board);
    void init_hash();
    void update_bitboards();
    void add_piece(uint8_t index, uint8_t color, uint8_t type);
    void remove_piece(uint8_t index, uint8_t color, uint8_t type);
    void break_castling(uint8_t color, uint8_t type);
    void invert_side();
public:
    explicit game_state(const std::string& fen);
    game_state(const game_state& state) = default;
    void apply_move(const chess_move& move);
    [[nodiscard]] bool is_check() const;
    [[nodiscard]] std::string fen() const;
};


#endif //CHESSUCIENGINE_GAME_STATE_H
