#ifndef CHESSUCIENGINE_STATIC_EVALUATOR_H
#define CHESSUCIENGINE_STATIC_EVALUATOR_H

#include <cstdint>
#include <array>
#include "game_state.h"

class static_evaluator {
    friend class dynamic_evaluator;
    static constexpr std::array<int32_t, 6> material_cost {
        950, // queen 
        0,   // king is unused
        563, // rook
        305, // knight
        333, // bishop
        100  // pawn
    };
    struct mobility {
        static constexpr int32_t Knight = 9;
        static constexpr int32_t Bishop = 4;
        static constexpr int32_t Rook = 3;
        static constexpr int32_t Queen = 3;
    };
    static constexpr int32_t DoublePawn = -25;
    static constexpr int32_t ConnectedPawn = 12;
    static constexpr std::array<int32_t, 8> DefaultPawnPromotion = {0, 0, 0, 0, 10, 20, 30, 0};
    static constexpr std::array<int32_t, 8> PassedPawnPromotion = {0, 50, 50, 50, 70, 90, 110, 0};
    static constexpr int32_t CrashedCastling = -50;
    static constexpr int32_t PawnShield = 33;
    static constexpr int32_t TwoBishops = 50;
    static constexpr int32_t MaximumPiecesForEndgame = 8;
    static constexpr int32_t AttackerKingProximityToDefenderKing = 10;
    static constexpr int32_t DistanceBetweenDefenderKingAndMiddle = 10;
    
    static int32_t material(const game_state& state);
    static int32_t mobility(const game_state& state);
    static int32_t double_pawn(const game_state& state);
    static int32_t connected_pawn(const game_state& state);
    static int32_t pawn_promotion(const game_state& state);
    static int32_t crashed_castling(const game_state& state);
    static int32_t pawn_shield(const game_state& state);
    static int32_t two_bishops(const game_state& state);
    
    static bool is_endgame(const game_state& state);
    static int32_t endgame_evaluation(const game_state& state, bool white_leading);
    static bool is_obvious_draw(const game_state& state);
public:
    static int32_t evaluate(const game_state& state);
};


#endif //CHESSUCIENGINE_STATIC_EVALUATOR_H
