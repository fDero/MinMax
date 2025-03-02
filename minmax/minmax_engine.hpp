//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <vector>
#include <utility>

#include "game_board.hpp"
#include "game_score.h"

template <game_score Score, game_board Board>
struct MinMaxEngine {

    [[nodiscard]] Board find_best_move(size_t max_depth, const Board& board) const {
        auto children = board.children();
        bool current_player_is_maximizing = board.current_player_is_maximizing();
        if (max_depth == 0 || children.empty()) {
            throw std::runtime_error("No moves found");
        }
        size_t best_move_index = 0;
        Score best_score_so_far = (current_player_is_maximizing)
            ? inf_limit<Score>()
            : sup_limit<Score>();
        for (size_t current_move_index = 0; current_move_index < children.size(); current_move_index++) {
            const auto& child = children[current_move_index];
            Score new_score = (board.current_player_is_maximizing())
                ? minimizing_score(max_depth, child, State())
                : maximizing_score(max_depth, child, State());
            if ((new_score > best_score_so_far) == current_player_is_maximizing) {
                best_score_so_far = new_score;
                best_move_index = current_move_index;
            }
        }
        return children[best_move_index];
    }

    struct State {
        Score global_maximum = inf_limit<Score>(); // alpha
        Score global_minimum = sup_limit<Score>(); // beta
    };

    [[nodiscard]] Score maximizing_score(size_t max_depth, const Board& board, State state) const {
        auto children = board.children();
        if (max_depth-- == 0 || children.empty()) {
            return board.evaluate();
        }
        Score local_maximum = inf_limit<Score>();
        for (const Board& child : children) {
            local_maximum = std::max(local_maximum, minimizing_score(max_depth, child, state));
            state.global_maximum = std::max(local_maximum, state.global_maximum);
            if (state.global_minimum <= state.global_maximum) {
                break;
            }
        }
        return local_maximum;
    }

    [[nodiscard]] Score minimizing_score(size_t max_depth, const Board& board, State state) const {
        auto children = board.children();
        if (max_depth-- == 0 || children.empty()) {
            return board.evaluate();
        }
        Score local_minimum = sup_limit<Score>();
        for (const Board& child : children) {
            local_minimum = std::min(local_minimum, maximizing_score(max_depth, child, state));
            state.global_minimum = std::min(local_minimum, state.global_minimum);
            if (state.global_minimum <= state.global_maximum) {
                break;
            }
        }
        return local_minimum;
    }
};
