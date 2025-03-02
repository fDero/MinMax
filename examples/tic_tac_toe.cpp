//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <numeric>

#include "minmax_engine.hpp"
#include "game_board.hpp"

struct TicTacToeBoard {

    using depth_t = int;
    using score_t = int;
    using move_t = int;

    enum class Square {
        EMPTY,
        OCCUPIED_X,
        OCCUPIED_O
    };

    static const move_t INITIAL_MOVE = -1;

    depth_t depth = 0;
    move_t prev_move = INITIAL_MOVE;
    std::array<Square, 9> internal = {
            Square::EMPTY, Square::EMPTY, Square::EMPTY,
            Square::EMPTY, Square::EMPTY, Square::EMPTY,
            Square::EMPTY, Square::EMPTY, Square::EMPTY,
    };

    explicit TicTacToeBoard(const std::array<Square, 9> internal) : internal(internal) {
        size_t x_count = std::count(internal.begin(), internal.end(), Square::OCCUPIED_X);
        size_t o_count = std::count(internal.begin(), internal.end(), Square::OCCUPIED_O);
        ensure(o_count <= x_count);
    }

    TicTacToeBoard() = default;
    TicTacToeBoard(const TicTacToeBoard&) noexcept = default;
    TicTacToeBoard(TicTacToeBoard&&) noexcept = default;

    TicTacToeBoard& operator=(const TicTacToeBoard& other) = default;
    TicTacToeBoard& operator=(TicTacToeBoard&& other) = default;

    bool operator==(const TicTacToeBoard& other) const noexcept = default;
    bool operator!=(const TicTacToeBoard& other) const noexcept = default;

    static void ensure(bool condition) {
        if (!condition) {
            throw std::runtime_error("Error: illegal state of the tic-tac-toe board");
        }
    }

    [[nodiscard]] score_t evaluate() const {

        using success_scenario = std::array<size_t, 3>;

        static success_scenario column1 = {0, 3, 6};
        static success_scenario column2 = {1, 4, 7};
        static success_scenario column3 = {2, 5, 8};

        static success_scenario diagonal1  = {0, 4, 8};
        static success_scenario diagonal2  = {2, 4, 6};

        static success_scenario row1 = {0, 1, 2};
        static success_scenario row2 = {3, 4, 5};
        static success_scenario row3 = {6, 7, 8};

        static std::array<success_scenario, 8> success_array = {
                column1,
                column2,
                column3,
                diagonal1,
                diagonal2,
                row1,
                row2,
                row3
        };

        for (success_scenario s : success_array) {
            size_t index1 = s[0];
            size_t index2 = s[1];
            size_t index3 = s[2];

            bool cond1 = internal[index1] == internal[index2];
            bool cond2 = internal[index2] == internal[index3];
            bool cond3 = internal[index3] == internal[index1];
            bool match = cond1 && cond2 && cond3;

            if (match && internal[index1] == Square::OCCUPIED_X) {
                return std::numeric_limits<score_t>::max() - depth;
            }

            if (match && internal[index1] == Square::OCCUPIED_O) {
                return std::numeric_limits<score_t>::min() + depth;
            }
        }

        return 0;
    }

    [[nodiscard]] std::vector<TicTacToeBoard> children() const {
        if (evaluate() != 0) {
            return {};
        }
        std::vector<TicTacToeBoard> moves;
        for (move_t i = 0; i < 9; i++) {
            if (internal[i] == Square::EMPTY) {
                auto new_internal = internal;
                new_internal[i] = current_player_is_maximizing()
                    ? Square::OCCUPIED_X
                    : Square::OCCUPIED_O;
                moves.emplace_back(new_internal);
                moves.back().prev_move = i;
                moves.back().depth = depth + 1;
            }
        }
        return moves;
    }

    [[nodiscard]] move_t get_prev_move() const {
        return prev_move;
    }

    [[nodiscard]] bool current_player_is_maximizing() const {
        size_t x_count = std::count(internal.begin(), internal.end(), Square::OCCUPIED_X);
        size_t o_count = std::count(internal.begin(), internal.end(), Square::OCCUPIED_O);
        return (x_count <= o_count);
    }

    friend std::ostream& operator<<(std::ostream& stream, const TicTacToeBoard& board) {
        std::string structure =
                "+---+---+---+ \n"
                "| @ | @ | @ | \n"
                "+---+---+---+ \n"
                "| @ | @ | @ | \n"
                "+---+---+---+ \n"
                "| @ | @ | @ | \n"
                "+---+---+---+ \n";

        uint8_t counter = 0;
        for (char& c : structure) {
            if (c == '@') {
                switch (board.internal[counter++]) {
                    break; case Square::OCCUPIED_X: c = 'X';
                    break; case Square::OCCUPIED_O: c = 'O';
                    break; case Square::EMPTY:      c = ' ';
                }
            }
        }

        stream << structure << "\n";
        return stream;
    }
};

static_assert(game_board<TicTacToeBoard>);
using TicTacToeEngine = MinMaxEngine<TicTacToeBoard::score_t, TicTacToeBoard>;

int main(int argc, [[maybe_unused]] char** argv) {
    assert (argc == 1);
    std::cout << "AUTOMATIC MODE" << std::endl;

    TicTacToeBoard board;
    TicTacToeEngine engine;

    while (!board.children().empty()) {
        board = engine.find_best_move(10, board);
        std::cout << board;
    }
}
