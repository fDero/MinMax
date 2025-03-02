//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <cstdint>
#include <concepts>
#include <vector>

template<typename GB>
concept game_board = requires (const GB& board) {
    { board.evaluate()                       } -> std::totally_ordered;
    { board.children()                       } -> std::same_as<std::vector<GB>>;
    { board == board                         } -> std::same_as<bool>;
    { board.current_player_is_maximizing()   } -> std::same_as<bool>;
};
