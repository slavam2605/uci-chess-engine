#include "static_evaluator.h"
#include "chess_utils.h"
#include "legal_move_mask.h"

using namespace std;

int32_t static_evaluator::evaluate(const game_state& state) {
    auto endgame = is_endgame(state);
    if (endgame && is_obvious_draw(state))
        return 0;

    int32_t value = 0;
    value += material(state);
    value += mobility(state);
    value += double_pawn(state);
    value += connected_pawn(state);
    value += pawn_promotion(state);
    value += crashed_castling(state);
    value += pawn_shield(state);
    value += two_bishops(state);
    
    if (endgame) {
        value += endgame_evaluation(state, value > 0);
    }
    return value;
}

int32_t static_evaluator::material(const game_state& state) {
    int32_t result = 0;
    for (int type = chess::Queen; type <= chess::Pawn; type++) {
        if (type == chess::King) continue;
        result += material_cost[type] * (count_1(state.board[chess::White][type]) - count_1(state.board[chess::Black][type]));
    }
    return result;
}

int32_t static_evaluator::mobility(const game_state& state) {
#define calc_mobility(figure) \
    for (int side = chess::Black; side <= chess::White; side++) {\
        while (board_copy[side][chess::figure]) {\
            uint8_t index = lsb(board_copy[side][chess::figure]);\
            set_0(board_copy[side][chess::figure], index);\
            auto mask = legal_move_mask::generate_figure_mask<chess::figure>(state, index, side, false);\
            result += sign[side] * mobility::figure * count_1(mask);\
        }\
    }

    int32_t result = 0;
    array<array<bitboard, 6>, 2> board_copy = state.board;
    int32_t sign[2] {-1, +1};

    calc_mobility(Knight)
    calc_mobility(Bishop)
    calc_mobility(Rook)
    calc_mobility(Queen)
    
    return result;
#undef calc_mobility
}

int32_t static_evaluator::double_pawn(const game_state& state) {
    int32_t result = 0;
    for (int col = 0; col < 8; col++) {
        uint8_t white_pawns = count_1(state.board[chess::White][chess::Pawn] & util_mask::cols[col]);
        uint8_t black_pawns = count_1(state.board[chess::Black][chess::Pawn] & util_mask::cols[col]);
        result += DoublePawn * (max(0, white_pawns - 1) - max(0, black_pawns - 1));
    }
    return result;
}

int32_t static_evaluator::connected_pawn(const game_state& state) {
    bitboard white_captures = legal_move_mask::generate_left_pawn_capture_mask(state, chess::White, true) | legal_move_mask::generate_right_pawn_capture_mask(state, chess::White, true);
    bitboard black_captures = legal_move_mask::generate_left_pawn_capture_mask(state, chess::Black, true) | legal_move_mask::generate_right_pawn_capture_mask(state, chess::Black, true);
    uint8_t white_count = count_1(white_captures & state.board[chess::White][chess::Pawn]);
    uint8_t black_count = count_1(black_captures & state.board[chess::Black][chess::Pawn]);
    return ConnectedPawn * (white_count - black_count);
}

int32_t static_evaluator::pawn_promotion(const game_state& state) {
    int32_t result = 0;
    auto white_pawns = state.board[chess::White][chess::Pawn];
    auto black_pawns = state.board[chess::Black][chess::Pawn];

    while (white_pawns) {
        uint8_t index = lsb(white_pawns);
        set_0(white_pawns, index);
        if (passed_pawn_masks::white_mask[index] & state.board[chess::Black][chess::Pawn]) 
            result += DefaultPawnPromotion[index / 8];
        else 
            result += PassedPawnPromotion[index / 8];
    }
    while (black_pawns) {
        uint8_t index = lsb(black_pawns);
        set_0(black_pawns, index);
        if (passed_pawn_masks::black_mask[index] & state.board[chess::White][chess::Pawn])
            result -= DefaultPawnPromotion[7 - index / 8];
        else
            result -= PassedPawnPromotion[7 - index / 8];
    }

    return result;
}

int32_t static_evaluator::crashed_castling(const game_state& state) {
    int32_t result = 0;
    if (!state.castling_happened[chess::White]) {
        if (!state.castling[chess::White][chess::King]) result += CrashedCastling;
        if (!state.castling[chess::White][chess::Queen]) result += CrashedCastling;
    }
    if (!state.castling_happened[chess::Black]) {
        if (!state.castling[chess::Black][chess::King]) result -= CrashedCastling;
        if (!state.castling[chess::Black][chess::Queen]) result -= CrashedCastling;
    }
    return result;
}

int32_t static_evaluator::pawn_shield(const game_state& state) {
    int32_t result = 0;
    if (state.castling_happened[chess::White]) {
        auto white_pawns = state.board[chess::White][chess::Pawn];
        auto king_index = lsb(state.board[chess::White][chess::King]);
        auto shield = pawn_shield_mask::white_mask[king_index];
        result += PawnShield * count_1(white_pawns & shield);
    }
    if (state.castling_happened[chess::Black]) {
        auto black_pawns = state.board[chess::Black][chess::Pawn];
        auto king_index = lsb(state.board[chess::Black][chess::King]);
        auto shield = pawn_shield_mask::black_mask[king_index];
        result -= PawnShield * count_1(black_pawns & shield);
    }
    return result;
}

int32_t static_evaluator::two_bishops(const game_state& state) {
    int32_t result = 0;
    if (count_1(state.board[chess::White][chess::Bishop]) >= 2) result += TwoBishops;
    if (count_1(state.board[chess::Black][chess::Bishop]) >= 2) result -= TwoBishops;
    return result;
}

bool static_evaluator::is_endgame(const game_state& state) {
    return count_1(state.all) <= MaximumPiecesForEndgame;
}

int32_t static_evaluator::endgame_evaluation(const game_state& state, bool white_leading) {
    int32_t result = 0;
    uint8_t attacker_side = white_leading ? chess::White : chess::Black;
    uint8_t defender_side = white_leading ? chess::Black : chess::White;
    
    uint8_t attacker_king_index = lsb(state.board[attacker_side][chess::King]);
    uint8_t attacker_king_x = attacker_king_index % 8;
    uint8_t attacker_king_y = attacker_king_index / 8;
    
    uint8_t defender_king_index = lsb(state.board[defender_side][chess::King]);
    uint8_t defender_king_x = defender_king_index % 8;
    uint8_t defender_king_y = defender_king_index / 8;
    
    result += AttackerKingProximityToDefenderKing * (16 - abs(attacker_king_x - defender_king_x) - abs(attacker_king_y - defender_king_y));
    result += DistanceBetweenDefenderKingAndMiddle * (abs(defender_king_x - 3) + abs(defender_king_y - 4));
    if (!white_leading) result *= -1;
    return result;
}

bool static_evaluator::is_obvious_draw(const game_state& state) {
    // Only two kings => draw
    uint8_t total_pieces = count_1(state.all);
    if (total_pieces == 2) return true;
    
    // Two kings and one light piece => draw
    bitboard heavy = state.board[chess::White][chess::Queen] | state.board[chess::Black][chess::Queen] |
                     state.board[chess::White][chess::Rook] | state.board[chess::Black][chess::Rook];
    bitboard pawns = state.board[chess::White][chess::Pawn] | state.board[chess::Black][chess::Pawn];
    if (total_pieces == 3 && heavy == 0 && pawns == 0) return true;
    
    // Two kings and two bishops of the same color
    if (total_pieces == 4 && heavy == 0 && pawns == 0) {
        bitboard white_bishops = state.board[chess::White][chess::Bishop];
        bitboard black_bishops = state.board[chess::Black][chess::Bishop];
        if (count_1(white_bishops) == 1 && count_1(black_bishops) == 1) {
            uint8_t white_index = lsb(white_bishops);
            uint8_t black_index = lsb(black_bishops);
            if (((white_index / 8 + white_index % 8) & 1) == ((black_index / 8 + black_index % 8) & 1))
                return true;
        }
    }
    
    return false;
}
