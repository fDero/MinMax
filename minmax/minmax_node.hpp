//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <vector>
#include <utility>

#include "game_board.hpp"

template <typename Move, game_board<Move> Board>
struct MinMaxNode {

    explicit MinMaxNode(const Board& board) : board(board) {}
    explicit MinMaxNode(Board&& board) : board(std::move(board)) {}

    MinMaxNode(const MinMaxNode& other) = default;
    MinMaxNode& operator=(const MinMaxNode& other) noexcept = default;

    MinMaxNode(MinMaxNode&& other) noexcept = default;
    MinMaxNode& operator=(MinMaxNode&& other) noexcept = default;

    [[nodiscard]] auto maximizing_score() const {
        if (children.empty()) {
            return board.evaluate();
        }
        auto maximum_so_far = children[0].minimizing_score();
        for (size_t i = 1; i < children.size(); i++) {
            auto minimizing_score = children[i].minimizing_score();
            maximum_so_far = std::max(maximum_so_far, minimizing_score);
        }
        return maximum_so_far;
    }

    [[nodiscard]] auto minimizing_score() const {
        if (children.empty()) {
            return board.evaluate();
        }
        auto minimum_so_far = children[0].maximizing_score();
        for (size_t i = 1; i < children.size(); i++) {
            auto maximizing_score = children[i].maximizing_score();
            minimum_so_far = std::min(minimum_so_far, maximizing_score);
        }
        return minimum_so_far;
    }

    [[nodiscard]] auto score(bool for_maximizing_player) const {
        return (for_maximizing_player)
            ? minimizing_score()
            : maximizing_score();
    }

    void make_children(size_t max_depth) {
        if (max_depth > 0) {
            make_children_helper();
            for (size_t i = 0; i < children.size(); i++) {
                children[i].make_children(max_depth - 1);
            }
        }
    }

    void refresh_children(size_t max_depth) {
        if (max_depth == 0) {
            return;
        }
        if (max_depth == 1) {
            make_children_helper();
            return;
        }
        for (size_t i = 0; i < children.size(); i++) {
            children[i].make_children(max_depth - 1);
        }
    }

    void make_children_helper() {
        assert (children.empty());
        auto moves = board.children();
        for (const Move& move : moves) {
            children.emplace_back(board.make(move));
        }
    }

    Board board;
    std::vector<MinMaxNode> children;
};
