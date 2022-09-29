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

    inline bitboard attackers_to(const game_state& state, bitboard occupied, uint8_t position, uint8_t side) {
        bitboard result = 0;
        auto inv_side = chess::inverse_color(side);
        auto pawn_mask = side == chess::White ? pawn_masks::white_mask[position] : pawn_masks::black_mask[position];
        
        result |= pawn_mask & state.board[inv_side][chess::Pawn];
        result |= legal_move_mask::generate_figure_mask<chess::Knight>(position, occupied) & state.board[inv_side][chess::Knight];
        result |= legal_move_mask::generate_figure_mask<chess::King>(position, occupied) & state.board[inv_side][chess::King];
        result |= legal_move_mask::generate_figure_mask<chess::Bishop>(position, occupied) & (state.board[inv_side][chess::Bishop] | state.board[inv_side][chess::Queen]);
        result |= legal_move_mask::generate_figure_mask<chess::Rook>(position, occupied) & (state.board[inv_side][chess::Rook] | state.board[inv_side][chess::Queen]);

        return result;
    }

    inline bool in_danger(const game_state& state, bitboard occupied, uint8_t position, uint8_t side) {
        return attackers_to(state, occupied, position, side);
    }

    inline bool is_legal(const chess_move& move, const game_state& state) {
        auto new_state = state;
        set_0(new_state.board[state.side][move::attacker(move)], move::from(move));
        set_1(new_state.board[state.side][move::attacker(move)], move::to(move));
        if (move::defender(move) != chess::EmptyPiece) {
            set_0(new_state.board[chess::inverse_color(state.side)][move::defender(move)], move::to(move));
        }
        if (move::flag(move) == move::move_flag::EnPassantCapture) {
            if (state.side == chess::White) {
                set_0(new_state.board[chess::Black][chess::Pawn], move::to(move) - 8);
            } else {
                set_0(new_state.board[chess::White][chess::Pawn], move::to(move) + 8);
            }
        }
        new_state.update_bitboards();
        auto king_position = lsb(new_state.board[state.side][chess::King]);
        return !in_danger(new_state, new_state.all, king_position, state.side);
    }
    
    template <uint8_t Figure>
    inline void generate_moves(move_list& moves, const game_state& state, uint8_t side,
                               uint8_t from, bitboard mask, bitboard pinned, uint8_t king_sq) {
        while (mask) {
            auto to = lsb(mask);
            set_0(mask, to);
            chess_move move = move::make_move(from, to, Figure, state.get_piece(chess::inverse_color(side), to));
            if constexpr (Figure == chess::King) {
                if (!in_danger(state, state.all ^ (1ULL << from), to, side)) {
                    moves.push_back(move);
                }
            } else {
                if (!get_bit(pinned, from) || aligned(from, to, king_sq)) {
                    moves.push_back(move);                }
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
                                    bool is_capture, bitboard mask, move::move_flag flag, bitboard pinned, uint8_t king_sq) {
        while (mask) {
            auto to = lsb(mask);
            set_0(mask, to);
            auto from = static_cast<uint8_t>(to + from_shift);
            auto defending_piece = is_capture ? state.get_piece(chess::inverse_color(side), to) : chess::EmptyPiece;
            chess_move move = move::make_move(from, to, chess::Pawn, defending_piece, flag);
            if (!get_bit(pinned, from) || aligned(from, to, king_sq)) {
                if (to < 8 || to > 55) {
                    moves.push_back(move::make_move(move, move::move_flag::PromoteToBishop));
                    moves.push_back(move::make_move(move, move::move_flag::PromoteToKnight));
                    moves.push_back(move::make_move(move, move::move_flag::PromoteToQueen));
                    moves.push_back(move::make_move(move, move::move_flag::PromoteToRook));
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
            generate_pawn_moves(moves, state, side, side == chess::White ? -8 : 8, false, short_moves & target, move::move_flag::Default, pinned, king_sq);
            generate_pawn_moves(moves, state, side, side == chess::White ? -16 : 16, false, long_moves & target, move::move_flag::PawnLongMove, pinned, king_sq);
        }
        auto left_captures = legal_move_mask::generate_left_pawn_capture_mask(state, side, false) & target;
        auto right_captures = legal_move_mask::generate_right_pawn_capture_mask(state, side, false) & target;
        generate_pawn_moves(moves, state, side, side == chess::White ? -7 : 9, true, left_captures, move::move_flag::Default, pinned, king_sq);
        generate_pawn_moves(moves, state, side, side == chess::White ? -9 : 7, true, right_captures, move::move_flag::Default, pinned, king_sq);
    }
    
    inline void generate_en_passant_moves(move_list& moves, const game_state& state, uint8_t side) {
        if (state.en_passant == chess::Empty) return;
        if (side == chess::White) {
            if (state.en_passant % 8 != 7 && get_bit(state.board[chess::White][chess::Pawn], state.en_passant - 7)) {
                chess_move move = move::make_move(state.en_passant - 7, state.en_passant, chess::Pawn, chess::EmptyPiece, move::move_flag::EnPassantCapture);
                if (is_legal(move, state)) moves.push_back(move);
            }
            if (state.en_passant % 8 != 0 && get_bit(state.board[chess::White][chess::Pawn], state.en_passant - 9)) {
                chess_move move = move::make_move(state.en_passant - 9, state.en_passant, chess::Pawn, chess::EmptyPiece, move::move_flag::EnPassantCapture);
                if (is_legal(move, state)) moves.push_back(move);
            }
        } else {
            if (state.en_passant % 8 != 0 && get_bit(state.board[chess::Black][chess::Pawn], state.en_passant + 7)) {
                chess_move move = move::make_move(state.en_passant + 7, state.en_passant, chess::Pawn, chess::EmptyPiece, move::move_flag::EnPassantCapture);
                if (is_legal(move, state)) moves.push_back(move);
            }
            if (state.en_passant % 8 != 7 && get_bit(state.board[chess::Black][chess::Pawn], state.en_passant + 9)) {
                chess_move move = move::make_move(state.en_passant + 9, state.en_passant, chess::Pawn, chess::EmptyPiece, move::move_flag::EnPassantCapture);
                if (is_legal(move, state)) moves.push_back(move);
            }
        }
    }

    inline void generate_castling_moves(move_list& moves, const game_state& state, uint8_t side) {
        auto index = side == chess::White ? 0 : 56; // index of the first cell in the castling row
        auto long_flag = side == chess::White ? move::move_flag::WhiteLongCastling : move::move_flag::BlackLongCastling;
        auto short_flag = side == chess::White ? move::move_flag::WhiteShortCastling : move::move_flag::BlackShortCastling;
        if (state.castling[side][chess::Queen] &&   // castling available => rook and king are on their positions 
            get_bit(state.empty, index + 1) &&      // |
            get_bit(state.empty, index + 2) &&      // | no figures between king and rook
            get_bit(state.empty, index + 3) &&      // |
            !in_danger(state, state.all, index + 2, side) &&   // king's target cell is not under attack
            !in_danger(state, state.all, index + 3, side) &&   // king's passing cell is not under attack
            !in_danger(state, state.all, index + 4, side)) {   // king itself is not under attack
            moves.push_back(move::make_move(index + 4, index + 2, chess::King, chess::EmptyPiece, long_flag));
        }
        if (state.castling[side][chess::King] &&    // castling available => rook and king are on their positions
            get_bit(state.empty, index + 5) &&      // |
            get_bit(state.empty, index + 6) &&      // | no figures between king and rook
            !in_danger(state, state.all, index + 4, side) &&   // king itself is not under attack
            !in_danger(state, state.all, index + 5, side) &&   // king's passing cell is not under attack
            !in_danger(state, state.all, index + 6, side)) {   // king's target cell is not under attack
            moves.push_back(move::make_move(index + 4, index + 6, chess::King, chess::EmptyPiece, short_flag));
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
        auto checkers = attackers_to(state, state.all, king_position, side);
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
    }
}

#endif //CHESSUCIENGINE_CHESS_MOVE_GENERATOR_H
