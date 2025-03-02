//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <optional>
#include <memory>
#include <stdexcept>
#include <cstdint>
#include <vector>
#include <random>
#include <iostream>

#include "game_board.hpp"
#include "minmax_node.hpp"

template <typename Move, game_board<Move> Board>
class MinMaxEngine {
    public:
        MinMaxEngine(Board board, size_t max_depth) : root(board) {
            root.make_children(max_depth);
            maximizing_player = board.maximizing();
            best_move = std::nullopt;
        }

        [[nodiscard]] Move peek_best_move() const {
            if (best_move.has_value()) {
                return best_move.value();
            }
            if (root.children.size() == 0) {
                throw std::runtime_error("No moves available");
            }
            size_t index = 0;
            auto score = root.children[0].score(maximizing_player);
            for (size_t i = 1; i < root.children.size(); i++) {
                auto new_score = root.children[i].score(maximizing_player);
                if ((new_score > score) == (maximizing_player)) {
                    score = new_score;
                    index = i;
                }
            }
            return root.children[index].board.last_move();
        }

        void update(size_t max_depth) {
            root.refresh_children(max_depth);
        }

        Move do_best_move() {
            best_move = peek_best_move();
            assert (best_move.has_value());
            return do_move(best_move.value());
        }

        Move do_random_move() {
            if (root.board.children().empty()) {
                throw std::runtime_error("No moves available");
            }
            static auto random_generator = std::ranlux24(std::random_device{}());
            auto moves = root.board.children();
            auto dist = std::uniform_int_distribution<int>(0, moves.size() - 1);
            Move random_move = moves[dist(random_generator)];
            return do_move(random_move);
        }

        Move do_move(const Move& move) {
            auto moved_board = root.board.make(move);
            best_move = std::nullopt;

            std::optional<MinMaxNode<Move, Board>> selected_node;

            for (size_t i = 0; i < root.children.size(); i++) {
                if (root.children[i].board == moved_board) {
                    selected_node.emplace(std::move(root.children[i]));
                    break;
                }
            }

            assert(selected_node.has_value());
            root = selected_node.value();
            maximizing_player = !maximizing_player;
            return move;
        }

    private:
        bool maximizing_player = true;
        mutable std::optional<Move> best_move = std::nullopt;
        MinMaxNode<Move, Board> root;
};
