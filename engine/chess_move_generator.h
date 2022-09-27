#ifndef CHESSUCIENGINE_CHESS_MOVE_GENERATOR_H
#define CHESSUCIENGINE_CHESS_MOVE_GENERATOR_H

#include <vector>
#include "chess_move.h"
#include "game_state.h"
#include "move_masks.h"
#include "chess_utils.h"
#include "legal_move_mask.h"
#include "move_list.h"
#include "bitboard_utils.h"

namespace chess_move_generator {
    using namespace std;

    inline bool in_danger(const game_state& state, uint8_t position, uint8_t side) {
        auto pawn_left_captures = legal_move_mask::generate_left_pawn_capture_mask(state, chess::inverse_color(side), true);
        auto pawn_right_captures = legal_move_mask::generate_right_pawn_capture_mask(state, chess::inverse_color(side), true);
        auto pawn_captures = pawn_left_captures | pawn_right_captures;
        if (get_bit(pawn_captures, position)) return true;
        
        if (legal_move_mask::generate_figure_mask<chess::Knight>(position, state.all) & state.board[chess::inverse_color(side)][chess::Knight]) return true;
        if (legal_move_mask::generate_figure_mask<chess::King>(position, state.all) & state.board[chess::inverse_color(side)][chess::King]) return true;
        if (legal_move_mask::generate_figure_mask<chess::Bishop>(position, state.all) & (state.board[chess::inverse_color(side)][chess::Bishop] | state.board[chess::inverse_color(side)][chess::Queen])) return true;
        if (legal_move_mask::generate_figure_mask<chess::Rook>(position, state.all) & (state.board[chess::inverse_color(side)][chess::Rook] | state.board[chess::inverse_color(side)][chess::Queen])) return true;
        
        return false;
    }

    inline bitboard attackers_to(const game_state& state, uint8_t position, uint8_t side) {
        bitboard result = 0;
        auto inv_side = chess::inverse_color(side);
        auto pawn_mask = side == chess::White ? pawn_masks::white_mask[position] : pawn_masks::black_mask[position];
        
        result |= pawn_mask & state.board[inv_side][chess::Pawn];
        result |= legal_move_mask::generate_figure_mask<chess::Knight>(position, state.all) & state.board[inv_side][chess::Knight];
        result |= legal_move_mask::generate_figure_mask<chess::King>(position, state.all) & state.board[inv_side][chess::King];
        result |= legal_move_mask::generate_figure_mask<chess::Bishop>(position, state.all) & (state.board[inv_side][chess::Bishop] | state.board[inv_side][chess::Queen]);
        result |= legal_move_mask::generate_figure_mask<chess::Rook>(position, state.all) & (state.board[inv_side][chess::Rook] | state.board[inv_side][chess::Queen]);

        return result;
    }

    inline bool is_legal(const chess_move& move, const game_state& state) {
        auto new_state = state;
        set_0(new_state.board[move.side][move.attacker_type], move.from);
        set_1(new_state.board[move.side][move.attacker_type], move.to);
        if (move.defender_type != chess::Empty) {
            set_0(new_state.board[chess::inverse_color(move.side)][move.defender_type], move.to);
        }
        if (move.flag == chess_move::move_flag::EnPassantCapture) {
            if (move.side == chess::White) {
                set_0(new_state.board[chess::Black][chess::Pawn], move.to - 8);
            } else {
                set_0(new_state.board[chess::White][chess::Pawn], move.to + 8);
            }
        }
        new_state.update_bitboards();
        auto king_position = lsb(new_state.board[move.side][chess::King]);
        return !in_danger(new_state, king_position, move.side);
    }
    
    template <uint8_t Figure>
    inline void generate_moves(move_list& moves, const game_state& state, uint8_t side,
                               uint8_t from, bitboard mask, bitboard pinned, uint8_t king_sq) {
        while (mask) {
            auto to = lsb(mask);
            set_0(mask, to);
            uint8_t defender_type = chess::Empty;
            for (int type = 0; type < 6; type++) {
                if (get_bit(state.board[chess::inverse_color(side)][type], to)) {
                    defender_type = type;
                    break;
                }
            }
            chess_move move(from, to, side, Figure, defender_type);
            if (!get_bit(pinned, from) || aligned(from, to, king_sq)) {
                moves.push_back(move);
            }
        }
    }

    template <uint8_t Figure>
    inline void generate_figure_moves(move_list& moves, const game_state& state, uint8_t side, bitboard pinned, uint8_t king_sq, bitboard target) {
        auto figure_board = state.board[side][Figure];
        while (figure_board) {
            auto index = lsb(figure_board);
            set_0(figure_board, index);
            auto allowed_moves = legal_move_mask::generate_figure_mask<Figure>(index, state.all) & target;
            generate_moves<Figure>(moves, state, side, index, allowed_moves, pinned, king_sq);
        }
    }

    template <>
    inline void generate_figure_moves<chess::Pawn>(move_list& moves, const game_state& state, uint8_t side, bitboard pinned, uint8_t king_sq, bitboard target) = delete;

    inline void generate_pawn_moves(move_list& moves, const game_state& state, uint8_t side, int8_t from_shift, 
                                    bool is_capture, bitboard mask, chess_move::move_flag flag, bitboard pinned, uint8_t king_sq) {
        while (mask) {
            auto to = lsb(mask);
            set_0(mask, to);
            uint8_t defender_type = chess::Empty;
            if (is_capture) {
                for (int type = 0; type < 6; type++) {
                    if (get_bit(state.board[chess::inverse_color(side)][type], to)) {
                        defender_type = type;
                        break;
                    }
                }
            }
            auto from = static_cast<uint8_t>(to + from_shift);
            chess_move move(from, to, side, chess::Pawn, defender_type, flag);
            if (!get_bit(pinned, from) || aligned(from, to, king_sq)) {
                if (to < 8 || to > 55) {
                    moves.push_back(chess_move(move, chess_move::move_flag::PromoteToBishop));
                    moves.push_back(chess_move(move, chess_move::move_flag::PromoteToKnight));
                    moves.push_back(chess_move(move, chess_move::move_flag::PromoteToQueen));
                    moves.push_back(chess_move(move, chess_move::move_flag::PromoteToRook));
                } else {
                    moves.push_back(move);
                }
            }
        }
    }
    
    inline void generate_figure_moves_pawn(move_list& moves, const game_state& state, uint8_t side, bool only_captures, bitboard pinned, uint8_t king_sq, bitboard target) {
        if (!only_captures) {
            auto short_moves = legal_move_mask::generate_short_pawn_mask(state, side);
            auto long_moves = legal_move_mask::generate_long_pawn_mask(state, side);
            generate_pawn_moves(moves, state, side, side == chess::White ? -8 : 8, false, short_moves & target, chess_move::move_flag::Default, pinned, king_sq);
            generate_pawn_moves(moves, state, side, side == chess::White ? -16 : 16, false, long_moves & target, chess_move::move_flag::PawnLongMove, pinned, king_sq);
        }
        auto left_captures = legal_move_mask::generate_left_pawn_capture_mask(state, side, false) & target;
        auto right_captures = legal_move_mask::generate_right_pawn_capture_mask(state, side, false) & target;
        generate_pawn_moves(moves, state, side, side == chess::White ? -7 : 9, true, left_captures, chess_move::move_flag::Default, pinned, king_sq);
        generate_pawn_moves(moves, state, side, side == chess::White ? -9 : 7, true, right_captures, chess_move::move_flag::Default, pinned, king_sq);
    }
    
    inline void generate_en_passant_moves(move_list& moves, const game_state& state, uint8_t side) {
        if (state.en_passant == chess::Empty) return;
        if (side == chess::White) {
            if (state.en_passant % 8 != 7 && get_bit(state.board[chess::White][chess::Pawn], state.en_passant - 7)) {
                chess_move move(state.en_passant - 7, state.en_passant, chess::White, chess::Pawn, chess::Empty, chess_move::move_flag::EnPassantCapture);
                moves.push_back(move);
            }
            if (state.en_passant % 8 != 0 && get_bit(state.board[chess::White][chess::Pawn], state.en_passant - 9)) {
                chess_move move(state.en_passant - 9, state.en_passant, chess::White, chess::Pawn, chess::Empty, chess_move::move_flag::EnPassantCapture);
                moves.push_back(move);
            }
        } else {
            if (state.en_passant % 8 != 0 && get_bit(state.board[chess::Black][chess::Pawn], state.en_passant + 7)) {
                chess_move move(state.en_passant + 7, state.en_passant, chess::Black, chess::Pawn, chess::Empty, chess_move::move_flag::EnPassantCapture);
                moves.push_back(move);
            }
            if (state.en_passant % 8 != 7 && get_bit(state.board[chess::Black][chess::Pawn], state.en_passant + 9)) {
                chess_move move(state.en_passant + 9, state.en_passant, chess::Black, chess::Pawn, chess::Empty, chess_move::move_flag::EnPassantCapture);
                moves.push_back(move);
            }
        }
    }

    inline void generate_castling_moves(move_list& moves, const game_state& state, uint8_t side) {
        auto index = side == chess::White ? 0 : 56; // index of the first cell in the castling row
        auto long_flag = side == chess::White ? chess_move::move_flag::WhiteLongCastling : chess_move::move_flag::BlackLongCastling;
        auto short_flag = side == chess::White ? chess_move::move_flag::WhiteShortCastling : chess_move::move_flag::BlackShortCastling;
        if (state.castling[side][chess::Queen] &&   // castling available => rook and king are on their positions 
            get_bit(state.empty, index + 1) &&      // |
            get_bit(state.empty, index + 2) &&      // | no figures between king and rook
            get_bit(state.empty, index + 3) &&      // |
            !in_danger(state, index + 2, side) &&   // king's target cell is not under attack
            !in_danger(state, index + 3, side) &&   // king's passing cell is not under attack
            !in_danger(state, index + 4, side)) {   // king itself is not under attack
            moves.push_back(chess_move(index + 4, index + 2, side, chess::King, chess::Empty, long_flag));
        }
        if (state.castling[side][chess::King] &&    // castling available => rook and king are on their positions
            get_bit(state.empty, index + 5) &&      // |
            get_bit(state.empty, index + 6) &&      // | no figures between king and rook
            !in_danger(state, index + 4, side) &&   // king itself is not under attack
            !in_danger(state, index + 5, side) &&   // king's passing cell is not under attack
            !in_danger(state, index + 6, side)) {   // king's target cell is not under attack
            moves.push_back(chess_move(index + 4, index + 6, side, chess::King, chess::Empty, short_flag));
        }
    }
    
    inline bitboard get_absolute_pinned(const game_state& state, uint8_t side) {
        bitboard result = 0;
        auto king_position = lsb(state.board[side][chess::King]);
        auto rook_board = state.board[chess::inverse_color(side)][chess::Rook];
        auto bishop_board = state.board[chess::inverse_color(side)][chess::Bishop];
        auto queen_board = state.board[chess::inverse_color(side)][chess::Queen];
        
        bitboard pinner = (legal_move_mask::xray_rook_attacks(state, state.side_board[side], king_position) & (rook_board | queen_board)) |
                (legal_move_mask::xray_bishop_attacks(state, state.side_board[side], king_position) & (bishop_board | queen_board));
        while (pinner) {
            int square = lsb(pinner);
            pinner &= pinner - 1;
            result |= in_between_mask::mask[square][king_position] & state.side_board[side];
        }
        return result;
    }

    inline void generate_all_moves(move_list& moves, const game_state& state, uint8_t side, bool only_captures = false) {
        Assert(moves.size() == 0)
        auto king_position = lsb(state.board[side][chess::King]);
        auto checkers = attackers_to(state, king_position, side);
        auto checkers_count = count_1(checkers);
        auto pinned = get_absolute_pinned(state, side);
        bitboard target = only_captures ? state.side_board[chess::inverse_color(side)]
                                        : state.inv_side_board[side];

        generate_figure_moves<chess::King>(moves, state, side, 0, king_position, target);

        // In case of double check only king can move
        if (checkers_count <= 1) {
            if (checkers_count == 1) {
                auto checker = lsb(checkers);
                target &= in_between_mask::mask[king_position][checker] | (1ULL << checker);
            }
            generate_figure_moves_pawn(moves, state, side, only_captures, pinned, king_position, target);
            generate_figure_moves<chess::Knight>(moves, state, side, pinned, king_position, target);
            generate_figure_moves<chess::Rook>(moves, state, side, pinned, king_position, target);
            generate_figure_moves<chess::Bishop>(moves, state, side, pinned, king_position, target);
            generate_figure_moves<chess::Queen>(moves, state, side, pinned, king_position, target);
            generate_en_passant_moves(moves, state, side);
            if (!checkers && !only_captures) {
                generate_castling_moves(moves, state, side);
            }
        }
        
        // Filter out non-legal moves
        int index = 0;
        for (int i = 0; i < moves.size(); i++) {
            bool should_check = (pinned && get_bit(pinned, moves[i].from)) || moves[i].from == king_position ||
                                moves[i].flag == chess_move::move_flag::EnPassantCapture;
            if (!should_check || is_legal(moves[i], state)) {
                if (i != index) moves[index] = moves[i];
                index++;
            }
        }
        moves.resize(index);
    }
}

#endif //CHESSUCIENGINE_CHESS_MOVE_GENERATOR_H
