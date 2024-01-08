#ifndef _ARCADE_COUNTER
#define _ARCADE_COUNTER

#include <graphics/graphics.hxx>

namespace Runtime {
    inline unsigned short high_score = 0;
    inline unsigned short current_score = 0;

    inline unsigned int live_count = 0;
    inline unsigned int coin_display = 0;

    constexpr unsigned int life_value = 25;
    constexpr float coin_display_multiplier     = 0.01;
    constexpr unsigned int extra_coin_divisor   = 5;
    constexpr unsigned int extra_coin_points    = 100;
    

    inline bool display_coins = true;
    
    void setupCounter();
    void fixedupdateCounter();
    void drawCounter();
}


#endif