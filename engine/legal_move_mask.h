#ifndef CHESSUCIENGINE_LEGAL_MOVE_MASK_H
#define CHESSUCIENGINE_LEGAL_MOVE_MASK_H

#include "move_masks.h"
#include "game_state.h"
#include "chess_utils.h"
#include "magic/magic_numbers.h"
#include "magic/magic_generator.h"

namespace legal_move_mask {
    inline bitboard generate_short_pawn_mask(const game_state& state, uint8_t side) {
        if (side == chess::White) {
            return (state.board[chess::White][chess::Pawn] << 8) & state.empty;
        }
        return (state.board[chess::Black][chess::Pawn] >> 8) & state.empty;
    }
    
    inline bitboard generate_long_pawn_mask(const game_state& state, uint8_t side) {
        auto short_mask = generate_short_pawn_mask(state, side);
        if (side == chess::White) {
            return ((short_mask & util_mask::rows[2]) << 8) & state.empty;
        }
        return ((short_mask & util_mask::rows[5]) >> 8) & state.empty;
    }
    
    inline bitboard generate_left_pawn_capture_mask(const game_state& state, uint8_t side, bool all_captures) {
        if (side == chess::White) {
            bitboard mask = (state.board[chess::White][chess::Pawn] << 7) & util_mask::inv_cols[7];
            if (!all_captures) mask &= state.side_board[chess::Black];
            return mask;
        }
        bitboard mask = (state.board[chess::Black][chess::Pawn] >> 9) & util_mask::inv_cols[7];
        if (!all_captures) mask &= state.side_board[chess::White];
        return mask;
    }

    inline bitboard generate_right_pawn_capture_mask(const game_state& state, uint8_t side, bool all_captures) {
        if (side == chess::White) {
            bitboard mask = (state.board[chess::White][chess::Pawn] << 9) & util_mask::inv_cols[0];
            if (!all_captures) mask &= state.side_board[chess::Black];
            return mask;
        }
        bitboard mask = (state.board[chess::Black][chess::Pawn] >> 7) & util_mask::inv_cols[0];
        if (!all_captures) mask &= state.side_board[chess::White];
        return mask;
    }

    template <uint8_t Figure>
    inline bitboard generate_figure_mask(uint8_t position, bitboard occupied) = delete;

    template <>
    inline bitboard generate_figure_mask<chess::King>(uint8_t position, bitboard occupied) {
        return king_mask::mask[position];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Knight>(uint8_t position, bitboard occupied) {
        return knight_mask::mask[position];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Rook>(uint8_t position, bitboard occupied) {
        auto block = occupied & magic_generator::rook_mask[position];
        auto index = (block * magic::rook_magic[position]) >> (64 - magic::rook_shift[position]);
        return magic_generator::rook_attack_masks[position][index];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Bishop>(uint8_t position, bitboard occupied) {
        auto block = occupied & magic_generator::bishop_mask[position];
        auto index = (block * magic::bishop_magic[position]) >> (64 - magic::bishop_shift[position]);
        return magic_generator::bishop_attack_masks[position][index];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Queen>(uint8_t position, bitboard occupied) {
        auto rook = generate_figure_mask<chess::Rook>(position, occupied);
        auto bishop = generate_figure_mask<chess::Bishop>(position, occupied);
        return rook | bishop;
    }
    
    inline bitboard xray_bishop_attacks(const game_state& state, bitboard blockers, uint8_t position) {
        auto attacks = generate_figure_mask<chess::Bishop>(position, state.all);
        blockers &= attacks;
        return attacks ^ generate_figure_mask<chess::Bishop>(position, state.all ^ blockers);
    }

    inline bitboard xray_rook_attacks(const game_state& state, bitboard blockers, uint8_t position) {
        auto attacks = generate_figure_mask<chess::Rook>(position, state.all);
        blockers &= attacks;
        return attacks ^ generate_figure_mask<chess::Rook>(position, state.all ^ blockers);
    }
}

#endif //CHESSUCIENGINE_LEGAL_MOVE_MASK_H
