//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Francesco De Rosa (https://github.com/fDero)                    //
// LICENSE: MIT (https://opensource.org/license/mit)                       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#pragma once

#include <cstdint>
#include <concepts>
#include <vector>

template<typename Score>
[[nodiscard]] Score sup_limit() {
    return std::numeric_limits<Score>::max();
}

template<typename Score>
[[nodiscard]] Score inf_limit() {
    return std::numeric_limits<Score>::min();
}

template<typename GS>
concept game_score = requires (const GS& score) {
    { score }             -> std::totally_ordered;
    { ::sup_limit<GS>() } -> std::same_as<GS>;
    { ::inf_limit<GS>() } -> std::same_as<GS>;
};

static_assert(game_score<size_t>);
static_assert(game_score<int8_t>);
static_assert(game_score<int16_t>);
static_assert(game_score<int32_t>);
static_assert(game_score<int64_t>);
static_assert(game_score<uint8_t>);
static_assert(game_score<uint16_t>);
static_assert(game_score<uint32_t>);
static_assert(game_score<uint64_t>);
static_assert(game_score<float>);
static_assert(game_score<double>);