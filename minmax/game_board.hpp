//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <cstdint>
#include <concepts>
#include <vector>

template<typename GB, typename Move>
concept game_board = requires (const GB& board, const Move& move, const GB& mut) {
    { board.evaluate()     } -> std::totally_ordered;
    { board.children()     } -> std::same_as<std::vector<Move>>;
    { board.make(move)     } -> std::convertible_to<GB>;
    { GB(board.make(move)) } -> std::same_as<GB>;
    { board == board       } -> std::same_as<bool>;
    { board.last_move()    } -> std::same_as<Move>;
    { board.maximizing()   } -> std::same_as<bool>;
};
