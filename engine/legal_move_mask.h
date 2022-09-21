#ifndef CHESSUCIENGINE_LEGAL_MOVE_MASK_H
#define CHESSUCIENGINE_LEGAL_MOVE_MASK_H

#include "move_masks.h"
#include "game_state.h"
#include "chess_utils.h"
#include "magic/magic_numbers.h"
#include "magic/magic_generator.h"

#define USE_MAGIC true

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
    inline bitboard generate_figure_mask(const game_state& state, uint8_t position, uint8_t side, bool only_captures) = delete;

    template <>
    inline bitboard generate_figure_mask<chess::King>(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
        if (only_captures) {
            return king_mask::mask[position] & state.side_board[chess::inverse_color(side)];
        }
        return king_mask::mask[position] & state.inv_side_board[side];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Knight>(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
        if (only_captures) {
            return knight_mask::mask[position] & state.side_board[chess::inverse_color(side)];
        }
        return knight_mask::mask[position] & state.inv_side_board[side];
    }
    
    inline bitboard generate_ray_mask(const game_state& state, uint8_t position, uint8_t side, 
                                      bool only_captures, uint8_t direction, bool is_lsb) {
        auto ray = slider_mask::mask[position][direction];
        auto blockers = ray & state.all;
        if (!blockers) {
            return only_captures ? 0 : ray;
        }
        auto blocking_square = is_lsb ? lsb(blockers) : msb(blockers);
        bitboard result = only_captures ? 0 : ray ^ slider_mask::mask[blocking_square][direction];
        if (get_bit(state.side_board[side], blocking_square)) {
            set_0(result, blocking_square);
        } else {
            set_1(result, blocking_square);
        }
        return result;
    }

    inline bitboard generate_magic_rook_mask(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
        auto block = state.all & magic_generator::rook_mask[position];
        auto index = (block * magic::rook_magic[position]) >> (64 - magic::rook_shift[position]);
        auto attack = magic_generator::rook_attack_masks[position][index];
        if (only_captures) {
            return attack & state.side_board[chess::inverse_color(side)];
        }
        return attack & state.inv_side_board[side];
    }

    inline bitboard generate_magic_bishop_mask(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
        auto block = state.all & magic_generator::bishop_mask[position];
        auto index = (block * magic::bishop_magic[position]) >> (64 - magic::bishop_shift[position]);
        auto attack = magic_generator::bishop_attack_masks[position][index];
        if (only_captures) {
            return attack & state.side_board[chess::inverse_color(side)];
        }
        return attack & state.inv_side_board[side];
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Rook>(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
#if USE_MAGIC
        return generate_magic_rook_mask(state, position, side, only_captures);
#else
        auto north = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::North, true);
        auto south = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::South, false);
        auto west = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::West, false);
        auto east = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::East, true);
        return north | south | west | east;
#endif
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Bishop>(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
#if USE_MAGIC
        return generate_magic_bishop_mask(state, position, side, only_captures);
#else
        auto nw = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::NorthWest, true);
        auto ne = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::NorthEast, true);
        auto sw = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::SouthWest, false);
        auto se = generate_ray_mask(state, position, side, only_captures, slider_mask::direction::SouthEast, false);
        return nw | ne | sw | se;
#endif
    }
    
    template <>
    inline bitboard generate_figure_mask<chess::Queen>(const game_state& state, uint8_t position, uint8_t side, bool only_captures) {
        auto rook = generate_figure_mask<chess::Rook>(state, position, side, only_captures);
        auto bishop = generate_figure_mask<chess::Bishop>(state, position, side, only_captures);
        return rook | bishop;
    }
}

#endif //CHESSUCIENGINE_LEGAL_MOVE_MASK_H
