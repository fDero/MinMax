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

struct Connect4Board {

    using score_t = int;
    using move_t = int;
    using depth_t = int;

    enum class Slot {
        EMPTY,
        OCCUPIED_X,
        OCCUPIED_O
    };

    enum class GameStatus {
        INCOMPLETE,
        DRAW,
        X_WIN,
        O_WIN,
    };

    enum class CurrentPlayer {
        PLAYER_X,
        PLAYER_O
    };

    static const move_t INITIAL_MOVE = -1;

    depth_t depth = 0;
    move_t prev_move = INITIAL_MOVE;
    std::array<Slot, 6*5> internal{};
    std::array<uint8_t, 6> heights{};
    CurrentPlayer current_player;

    explicit Connect4Board(const  std::array<Slot, 6*5>& internal)
        : internal(internal)
        , heights({0, 0, 0, 0, 0, 0})
    {
        for(size_t row = 0; row < 5; row++) {
            for (size_t col = 0; col < 6; col++) {
                size_t current_slot_index = row * 6 + col;
                if (internal[current_slot_index] != Slot::EMPTY) {
                    heights[col] = row + 1;
                }
            }
        }
        size_t x_count = std::count(internal.begin(), internal.end(), Slot::OCCUPIED_X);
        size_t o_count = std::count(internal.begin(), internal.end(), Slot::OCCUPIED_O);
        current_player = (x_count <= o_count)? CurrentPlayer::PLAYER_X : CurrentPlayer::PLAYER_O;
    }

    Connect4Board() noexcept {
        std::fill(internal.begin(), internal.end(), Slot::EMPTY);
        std::fill(heights.begin(), heights.end(), 0);
        current_player = CurrentPlayer::PLAYER_X;
    }

    Connect4Board(const Connect4Board&) noexcept = default;
    Connect4Board(Connect4Board&&) noexcept = default;

    Connect4Board& operator=(const Connect4Board& other) = default;
    Connect4Board& operator=(Connect4Board&& other) = default;

    bool operator==(const Connect4Board& other) const noexcept = delete;
    bool operator!=(const Connect4Board& other) const noexcept = delete;

    static void ensure(bool condition) {
        if (!condition) {
            throw std::runtime_error("Error: illegal state of the tic-tac-toe board");
        }
    }

    [[nodiscard]] GameStatus compute_game_status() const {

        // horizontal sequences
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col <= 2; ++col) {
                int idx = row * 6 + col;
                if (
                    internal[idx] != Slot::EMPTY
                    && internal[idx] == internal[idx + 1]
                    && internal[idx + 1] == internal[idx + 2]
                    && internal[idx + 2] == internal[idx + 3]
                )
                switch (internal[idx]) {
                    case Slot::OCCUPIED_X: return GameStatus::X_WIN;
                    case Slot::OCCUPIED_O: return GameStatus::O_WIN;
                    case Slot::EMPTY: exit(-1);
                }
            }
        }

        // vertical sequences
        for (int col = 0; col < 6; ++col) {
            for (int row = 0; row <= 1; ++row) {
                int idx = row * 6 + col;
                if (
                    internal[idx] != Slot::EMPTY
                    && internal[idx] == internal[idx + 6]
                    && internal[idx + 6] == internal[idx + 12]
                    && internal[idx + 12] == internal[idx + 18]
                )
                switch (internal[idx]) {
                    case Slot::OCCUPIED_X: return GameStatus::X_WIN;
                    case Slot::OCCUPIED_O: return GameStatus::O_WIN;
                    case Slot::EMPTY: exit(-1);
                }
            }
        }

        // diagonal down-right
        for (int row = 0; row <= 1; ++row) {
            for (int col = 0; col <= 2; ++col) {
                int idx = row * 6 + col;
                if (
                    internal[idx] != Slot::EMPTY
                    && internal[idx] == internal[idx + 7]
                    && internal[idx + 7] == internal[idx + 14]
                    && internal[idx + 14] == internal[idx + 21]
                )
                switch (internal[idx]) {
                    case Slot::OCCUPIED_X: return GameStatus::X_WIN;
                    case Slot::OCCUPIED_O: return GameStatus::O_WIN;
                    case Slot::EMPTY: exit(-1);
                }
            }
        }

        // diagonal up-right
        for (int row = 3; row <= 4; ++row) {
            for (int col = 0; col <= 2; ++col) {
                int idx = row * 6 + col;
                if (
                    internal[idx] != Slot::EMPTY
                    && internal[idx] == internal[idx - 5]
                    && internal[idx - 5] == internal[idx - 10]
                    && internal[idx - 10] == internal[idx - 15]
                )
                switch (internal[idx]) {
                    case Slot::OCCUPIED_X: return GameStatus::X_WIN;
                    case Slot::OCCUPIED_O: return GameStatus::O_WIN;
                    case Slot::EMPTY: exit(-1);
                }
            }
        }

        return (depth == 6*5)
            ? GameStatus::DRAW
            : GameStatus::INCOMPLETE;
    }

    [[nodiscard]] score_t evaluate() const {
        switch (compute_game_status()) {
            case GameStatus::DRAW:  return 0;
            case GameStatus::X_WIN: return std::numeric_limits<score_t>::max();
            case GameStatus::O_WIN: return std::numeric_limits<score_t>::min();
            case GameStatus::INCOMPLETE: break;
        }

        score_t score = 0;

        for (size_t move = 1; move <= 4; move++) {
            size_t index = move + 6*(heights[move] - 1);
            if (index >= 30) {
                continue;
            }
            score += (internal[index] == Slot::OCCUPIED_X) && (internal[index+1] == Slot::EMPTY);
            score += (internal[index] == Slot::OCCUPIED_X) && (internal[index-1] == Slot::EMPTY);
            score -= (internal[index] == Slot::OCCUPIED_O) && (internal[index+1] == Slot::EMPTY);
            score -= (internal[index] == Slot::OCCUPIED_O) && (internal[index-1] == Slot::EMPTY);
        }

        for (size_t move = 0; move <= 5; move++) {
            size_t index = move + 6*(heights[move] - 1);
            if (index >= 30) {
                continue;
            }
            if (index >= 6) {
                score += (internal[index] == Slot::OCCUPIED_X) && (internal[index - 6] == Slot::EMPTY);
                score -= (internal[index] == Slot::OCCUPIED_O) && (internal[index - 6] == Slot::EMPTY);
            }
            if (index <= 23) {
                score += (internal[index] == Slot::OCCUPIED_X) && (internal[index + 6] == Slot::EMPTY);
                score -= (internal[index] == Slot::OCCUPIED_O) && (internal[index + 6] == Slot::EMPTY);
            }
        }

        return score;
    }

    [[nodiscard]] std::vector<Connect4Board> children() const {
        if (compute_game_status() != GameStatus::INCOMPLETE) {
            return {};
        }
        std::vector<Connect4Board> children;
        for (move_t move = 0; move < 6; move++) {
            if (heights[move] < 5) {
                children.emplace_back(this->make(move));
            }
        }
        return children;
    }

    [[nodiscard]] move_t get_prev_move() const {
        return prev_move;
    }

    [[nodiscard]] Connect4Board make(move_t move) const {
        ensure(move >= 0 && move < 6);
        ensure(heights[move] < 5);
        auto new_internal = internal;
        size_t index = move + 6*heights[move];
        new_internal[index] = (current_player == CurrentPlayer::PLAYER_X)
            ? Slot::OCCUPIED_X
            : Slot::OCCUPIED_O;
        Connect4Board new_board(new_internal);
        new_board.depth = depth + 1;
        new_board.prev_move = move;
        new_board.current_player = (current_player == CurrentPlayer::PLAYER_X)
            ? CurrentPlayer::PLAYER_O
            : CurrentPlayer::PLAYER_X;
        return new_board;
    }

    [[nodiscard]] bool current_player_is_maximizing() const {
        return current_player == CurrentPlayer::PLAYER_X;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Connect4Board& board) {
        for(size_t row = 5; row != 0; row--) {
            stream << "|";
            for (size_t col = 0; col < 6; col++) {
                size_t current_slot_index = (row-1) * 6 + col;
                switch(board.internal[current_slot_index]) {
                    break; case Slot::OCCUPIED_X: stream << " X |";
                    break; case Slot::OCCUPIED_O: stream << " O |";
                    break; case Slot::EMPTY:      stream << "   |";
                }
            }
            stream << "\n";
        }
        stream << "\n\n";
        return stream;
    }
};

static_assert(game_board<Connect4Board>);
using Connect4Engine = MinMaxEngine<Connect4Board::score_t, Connect4Board>;

int main(int argc, [[maybe_unused]] char** argv) {
    assert (argc == 1);

    std::cout << "=============INTERACTIVE MODE=============" << std::endl;
    std::cout << "Rules of `CONNECT4` can be viewed at:     " << std::endl;
    std::cout << "https://en.wikipedia.org/wiki/Connect_Four" << std::endl;
    std::cout << std::endl << std::endl;

    Connect4Board board;
    Connect4Engine engine;

    while (!board.children().empty()) {
        try {
            std::cout << board << std::endl;
            auto board_after_best_move = engine.find_best_move(5, board);
            std::cout << "Best move: " << board_after_best_move.get_prev_move() << std::endl;
            std::cout << "Select a move [0-5]: ";
            std::string move_str;
            std::getline(std::cin, move_str);
            auto move = std::stoi(move_str);
            board = board.make(move);
        }
        catch (...) {
            std::cout << "[!!] Error, please try again" << std::endl << std::endl;
        }
    }

    std::cout << board << std::endl;
    std::cout << "Game ended" << std::endl;
 }