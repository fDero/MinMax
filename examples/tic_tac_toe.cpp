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

using TicTacToeMove = int;
using depth_t = int;

struct TicTacToeBoard {

    static const TicTacToeMove INITIAL_MOVE = -1;

    depth_t depth = 0;
    TicTacToeMove prev_move = INITIAL_MOVE;
    std::array<char, 9> internal = {
            '-', '-', '-',
            '-', '-', '-',
            '-', '-', '-',
    };

    explicit TicTacToeBoard(const std::array<char, 9> internal) : internal(internal) {
        size_t x_count = std::count(internal.begin(), internal.end(), 'X');
        size_t o_count = std::count(internal.begin(), internal.end(), 'O');
        ensure(o_count <= x_count);
        for (const char c : internal) {
            ensure(c == '-' || c == 'X' || c == 'O');
        }
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

    [[nodiscard]] int evaluate() const {

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

            if (match && internal[index1] == 'X') {
                return std::numeric_limits<int>::max() - depth;
            }

            if (match && internal[index1] == 'O') {
                return std::numeric_limits<int>::min() + depth;
            }
        }

        return 0;
    }

    [[nodiscard]] std::vector<TicTacToeMove> children() const {
        if (evaluate() != 0) {
            return {};
        }
        std::vector<TicTacToeMove> moves;
        for (TicTacToeMove i = 0; i < 9; i++) {
            if (internal[i] == '-') {
                moves.push_back(i);
            }
        }
        return moves;
    }

    [[nodiscard]] bool maximizing() const {
        size_t x_count = std::count(internal.begin(), internal.end(), 'X');
        size_t o_count = std::count(internal.begin(), internal.end(), 'O');
        return (x_count <= o_count);
    }

    [[nodiscard]] TicTacToeBoard make(TicTacToeMove move) const {
        ensure(internal[move] == '-');
        size_t x_count = std::count(internal.begin(), internal.end(), 'X');
        size_t o_count = std::count(internal.begin(), internal.end(), 'O');
        char player = (x_count > o_count)? 'O' : 'X';
        TicTacToeBoard tic_tac_toe_board;
        tic_tac_toe_board.internal = internal;
        tic_tac_toe_board.internal[move] = player;
        tic_tac_toe_board.prev_move = move;
        tic_tac_toe_board.depth = depth + 1;
        return tic_tac_toe_board;
    }

    [[nodiscard]] TicTacToeMove last_move() const {
        ensure(prev_move != INITIAL_MOVE);
        return prev_move;
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

        size_t counter = 0;
        for (char& c : structure) {
            if (c == '@') {
                c = (board.internal[counter] != '-')
                    ? board.internal[counter]
                    : ' ';
                counter++;
            }
        }

        stream << structure << "\n";
        return stream;
    }
};

static_assert(game_board<TicTacToeBoard, TicTacToeMove>);
using TicTacToeEngine = MinMaxEngine<TicTacToeMove, TicTacToeBoard>;

int main(int argc, [[maybe_unused]] char** argv) {
    assert (argc == 1);
    std::cout << "INTERACTIVE MODE" << std::endl;
    auto board = TicTacToeBoard();
    while (!board.children().empty()) {
        try {
            std::cout << board << std::endl;
            TicTacToeEngine engine(board, 10);
            std::cout << "Best move: " << engine.peek_best_move() << "\n";
            std::cout << "Select move [0-8]: ";
            std::string move_str;
            std::getline(std::cin, move_str);
            TicTacToeMove move = std::stoi(move_str);
            engine.do_move(move);
            board = board.make(move);
        }
        catch (...) {
            std::cout << "Error, please try again\n" << std::endl;
        }
    }
    std::cout << board;
    std::cout << "Game ended" << std::endl;
}
