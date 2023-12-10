#pragma once


#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <chrono>

namespace Runtime {
    using clock = std::chrono::steady_clock;
    using duration = clock::duration;
    using duration_ms = std::chrono::duration<unsigned long long, std::milli>;

    inline auto delta_time = duration::zero();
    inline auto current_time = clock::now();

    inline auto tick_length = std::chrono::duration_cast<clock::duration>(duration_ms(17));
    inline unsigned long long current_tick = 0;
} // namespace Math






